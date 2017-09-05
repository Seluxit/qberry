#include "reactor.hpp"
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include "error.hpp"

#include <chrono>
#include <thread>
#include "network.hpp"
#include "device.hpp"
#include "value.hpp"
#include "state.hpp"

#define BUF_SIZE 4048

namespace berry {
    
    Reactor* Reactor::instance()
    {
        static Reactor instance_;
        return &instance_;
    }

    Reactor::~Reactor()
    {

    }

    void Reactor::init()
    {
	fdescriptor_ = create_socket();
		
        wconnection_.set(fdescriptor_, ev::READ);
        wconnection_.set<Reactor, &Reactor::socket_callback>(this);
        wconnection_.start();

        load_elements();
		// Send network UUID to the data collector.
    }
    
    int Reactor::create_socket() 
    {
        int sockfd;
        char* tmp_ptr = NULL;
        struct hostent *host;
        struct sockaddr_in dest_addr;

        // Extract hostname and port from url 'https://example.com:12345'
        if ( (host = gethostbyname(hostname_.c_str())) == NULL ) {
            std::cerr << "Error: Cannot resolve hostname " <<  hostname_ << "\n";
            return 0;
        }

        // create the basic TCP socket        
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        dest_addr.sin_family=AF_INET;
        dest_addr.sin_port=htons(port_);
        dest_addr.sin_addr.s_addr = *(long*)(host->h_addr);

        /* ---------------------------------------------------------- *
        * Zeroing the rest of the struct                             *
        * ---------------------------------------------------------- */
        memset(&(dest_addr.sin_zero), '\0', 8);
        tmp_ptr = inet_ntoa(dest_addr.sin_addr);

        /* ---------------------------------------------------------- *
        * Try to make the host connect here                          *
        * ---------------------------------------------------------- */
        if ( ::connect(sockfd, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr)) == -1 ) {
            std::cerr << "Error: Cannot connect to host " << hostname_ << "  on port " <<  port_ << "\n";
            return 0;
        }

        return sockfd;
    }
    
    void Reactor::load_elements()
    {
        std::cout << "\n";
        std::cout << "------------- Sending all data to Q server -----------------\n";
        std::cout << "\n";

/*
 *        const std::string net_id = "01b58e53-ea40-4b28-9a5d-dd824e9333c2";
 *        network = std::shared_ptr<Network>(new Network(net_id)); 
 *        this->post(network);
 *
 *        const std::string dev_id = "01eb0f96-ecaf-4ae2-a677-13ee87d6b376";
 *        device = std::shared_ptr<Device>(new Device(network, dev_id));
 *        this->post(device);
 *
 *        // --------------------- Value 1 -------------------------
 *        const std::string val_id = "b71b9a80-dd26-4bdd-9fd8-d34df85e730f";
 *        auto value1 = std::shared_ptr<Value>(new Value(device, val_id));
 *
 *        value1->permission = "r";
 *        value1->name = "Switch";
 *        value1->type = "Switch state";
 *        value1->min = 0;
 *        value1->max = 1;
 *        value1->step = 1;
 *
 *        // ------------------ State 1 ----------------------------------
 *        auto pin17 = std::make_shared<berry::Gpio>(17, berry::Direction::in); 
 *        const std::string state_id = "a14416ea-2585-4f1f-bbdf-c41af40ed32f";
 *        auto state1 = std::shared_ptr<State>(new State(value1, state_id, pin17));
 *        state1->timestamp = berry::timestampToDateTime(berry::timestamp()); 
 *        state1->type = "Report";
 *        state1->data = "1";
 *
 *        this->values.push_back(value1);
 *        this->states.push_back(state1);
 *        this->post(value1);
 *        this->post(state1);
 *
 *
 *        // --------------------- Value 2 -------------------------
 *        const std::string val_id2 = "2fafd810-85ff-4468-9758-9c0ab22592ca";
 *        auto value2 = std::shared_ptr<Value>(new Value(device, val_id2));
 *
 *        value2->permission = "w";
 *        value2->name = "LED";
 *        value2->type = "LED state";
 *        value2->min = 0;
 *        value2->max = 1;
 *        value2->step = 1;
 *    
 *        // --------------------- State 2 -------------------------
 *        auto pin4 = std::make_shared<berry::Gpio>(4, berry::Direction::out); 
 *        const std::string state_id2 = "9e17d28c-a2df-401f-bcc7-27f307af01fd";
 *
 *        auto state2 = std::shared_ptr<State>(new State(value2, state_id2, pin4));
 *        state2->timestamp = berry::timestampToDateTime(berry::timestamp()); 
 *        state2->type = "Control";
 *        state2->data = "0";
 *
 *
 *        this->values.push_back(value2);
 *        this->states.push_back(state2);
 *        this->post(value2);
 *        this->post(state2);
 */
       
    }

    void Reactor::socket_callback(ev::io& w, int revents)
    {
        std::string msg = read();
        if (msg[0] =='\0' && msg.size() == BUF_SIZE) {
            std::cout << "Are server down? Re-connect\n";
            return; 
        }
    
        json root; 
        
        try {
            root = json::parse(msg);
        }
        catch(std::exception& e) {
            std::cout << "Parsing errr << " << e.what() << "\n";
            return;
        }

        // If PING received - send "PONG" immediately 
        if (root.at("id").get<std::string>() == "PING") {
            std::string ajson = R"({
                    "jsonrpc": "2.0", 
                    "result": true, 
                    "id": "PONG" 
                    })";

            std::cout << "PING received -- sending back PONG\n"; 
            write(ajson);
            return;
        }
        
        std::string msgId = root.at("id").get<std::string>();

        // check first for error message.
        if (root.find("method") != root.end() ) {
            
            std::string method = root["method"].get<std::string>();
            std::string url = root["params"]["url"].get<std::string>();

            std::string id = root["params"]["data"][":id"].get<std::string>();

            auto it = std::find_if(states.begin(), states.end(), 
                                  [&id](const std::shared_ptr<State>& state)
                                  {
                                        return state->id == id; 
                                  });
            
            std::stringstream ss;
            if (it == states.end()) {
                ss <<  R"({"jsonrpc": "2.0", "result": false, id": ")" << msgId;
                ss << R"("})"; 
                std::cout << "No such State ID: " << id << "\n"; 
                write(ss.str());

            }
            else {
                std::string data = root["params"]["data"]["data"].get<std::string>();
                ss <<  R"({"jsonrpc": "2.0", "result": true, id": ")" << msgId;
                ss << R"("})"; 
                std::cout << "Set Value: " << data << "\n"; 

                std::string ret = ss.str();
                std::cout << "Return " << ret << "\n";

                write(ss.str());

                (*it)->set(data);
            }
        }
    }

    void Reactor::write(const std::string& msg) const
    {
        ::send(fdescriptor_, msg.data(), msg.size(), 0);  
    }

    std::string Reactor::read() const
    {
        unsigned char data[BUF_SIZE];
        memset(data, 0, BUF_SIZE);

        int n = recv(fdescriptor_, data, BUF_SIZE, 0);
        if (n < 0) {
            std::cerr << std::string(" ERROR reading from socket!");
        }
        data[n] = '\0';    
        std::string message(data, data + sizeof data / sizeof data[0]);
        
        return message;
    }
    


    //bool Reactor::post

    ev::default_loop& Reactor::defaultLoop()
    {
        return loop_;
    }

} // namespace 
