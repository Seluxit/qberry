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

    void Reactor::init(const Conf_t& conf, const std::string& network_uuid)
    {
        conf_ = conf;
        
        std::string hostname;
        int port;
        std::tie(hostname, port) = hostname_and_port(conf_.SERVER);

        break_.set<Reactor, &Reactor::break_callback>(this);
        break_.start(SIGINT);

	    fdescriptor_ = create_socket(hostname, port);

        wconnection_.set(fdescriptor_, ev::READ);
        wconnection_.set<Reactor, &Reactor::socket_callback>(this);
        wconnection_.start();
        
        load_elements(network_uuid);
    }
    
    void Reactor::break_callback(ev::sig& signal, int revent)
    {
        for (auto& state : states) {
            state->unexport_pin();
        }

        signal.loop.break_loop();
        std::cout << "\nYou are killing Me! Who gave you such rights?\n";
    }

    const Conf_t& Reactor::configuration()
    {
        return conf_;
    }
    
    std::tuple<std::string, int> Reactor::hostname_and_port(const std::string& url)
    {
        std::string hostname = url;
       
        // remove protocol -
        std::string::size_type pos = hostname.find("://");
        if (pos != std::string::npos)
            hostname = hostname.substr(pos+3);            

        if (hostname.back() == '/')           
            hostname.pop_back();
        
        pos = hostname.find(":");
        std::string portnum = hostname.substr(pos+1);
        hostname = hostname.substr(0,pos);

        if (portnum.back() == '/')
            portnum.pop_back();
        
        return std::make_tuple(hostname, std::stoi(portnum));
    }

    int Reactor::create_socket(const std::string& hostname, int port) 
    {
        int sockfd;
        char* tmp_ptr = NULL;
        struct hostent *host;
        struct sockaddr_in dest_addr;

        // Extract hostname and port from url 'https://example.com:12345'
        if ( (host = gethostbyname(hostname.c_str())) == NULL ) {
            std::cerr << "Error: Cannot resolve hostname " <<  hostname << "\n";
            return 0;
        }

        // create the basic TCP socket        
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        dest_addr.sin_family=AF_INET;
        dest_addr.sin_port=htons(port);
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
            std::cerr << "Error: Cannot connect to host " << hostname << "  on port " <<  port << "\n";
            return 0;
        }

        return sockfd;
    }
    
    void Reactor::load_elements(const std::string& network_uuid)
    {
        // Loading all data recursively.
        network = std::make_shared<Network>(network_uuid); 

        std::cout << "\n";
        std::cout << "------------- Sending all data to Q server -----------------\n";
        std::cout << "\n";
 
        this->send("POST", network);
        for (const auto& device : this->devices) {
            send("POST", device);
        }
        for (const auto& value : this->values) {
            send("POST", value);
        }
        for (const auto& state : this->states) {
            send("POST", state);
        }
    }

    void Reactor::socket_callback(ev::io& w, int revents)
    {
        std::string msg = read();
        if (msg[0] =='\0' && msg.size() == BUF_SIZE) {
            std::cout << "Are server down? Re-connect\n";
            wconnection_.stop();
            // TODO --- timeout
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
        if (root.find("method") != root.end() && root["method"].get<std::string>() == "PUT") {
            
            //std::string method = root["method"].get<std::string>();
            std::string url = root["params"]["url"].get<std::string>();
            std::string id = root["params"]["data"][":id"].get<std::string>();

            auto it = std::find_if(states.begin(), states.end(), 
                                  [&id](const std::shared_ptr<State>& state)
                                  {
                                        return state->id == id; 
                                  });
            
            std::stringstream ss;
            if (it == states.end()) {
                ss <<  R"({"jsonrpc": "2.0", "result": false, "id": ")" << msgId;
                ss << R"("})"; 
                std::cout << "No such State with ID: " << id << "\n"; 
                write(ss.str());

            }
            else {
                std::string data = root["params"]["data"]["data"].get<std::string>();
                ss <<  R"({"jsonrpc": "2.0", "result": true, "id": ")" << msgId;
                ss << R"("})"; 
                std::cout << "Set Value: " << data << "\n"; 

                std::string ret = ss.str();
                std::cout << "Return " << ret << "\n";

                write(ss.str());

                (*it)->set(data);
            }
        }
        else {
            std::string ajson = R"({
                    "jsonrpc": "2.0", 
                    "result": false, 
                    "id": "Unknown" 
                    })";

            std::cout << ajson << "\n"; 
            write(ajson);
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
    
    ev::default_loop& Reactor::defaultLoop()
    {
        return loop_;
    }

} // namespace 
