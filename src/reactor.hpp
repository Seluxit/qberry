#pragma once

#include "gpio.hpp"
#include <ev++.h>
#include <memory>
#include <iostream>
#include <tuple>
#include "utils.hpp"
#include "configuration.hpp"

namespace berry {

class Network;
class Device;
class Value;
class State;

class Reactor
{
    private:
        Reactor() = default;
        ~Reactor();
        Reactor(const Reactor&) = delete;
        Reactor& operator=(const Reactor&) = delete;

        ev::sig break_;
        ev::io wconnection_;
        ev::default_loop loop_;
        
        int fdescriptor_ = 0;
        Conf_t conf_;

        std::tuple<std::string, int> hostname_and_port(const std::string& url);
    	int create_socket(const std::string& hostname, int port);
        void socket_callback(ev::io& w, int revents);
        void break_callback(ev::sig& w, int revent);
        void load_elements(const std::string& network_uuid);

    public:
        static Reactor* instance();
        void init(const Conf_t& conf, const std::string& network_uuid);
        
        const Conf_t& configuration();

        std::shared_ptr<Network> network;
        std::vector<std::shared_ptr<Device>> devices;
        std::vector<std::shared_ptr<Value>> values;
        std::vector<std::shared_ptr<State>> states;

        std::string read() const;
        void write(const std::string& msg) const;
        ev::default_loop& defaultLoop();

        template<typename T>
        void send(const std::string& method, const T& object)
        {
            assert(method == "POST" || method == "PUT"); 

            std::string url;
            if (method == "PUT")
                url = object->location() + object->id;
            else // POST
                url = object->location();

            std::string payload = jsonRpc(method, object->toJson(), url);
            write(payload);            
            
            std::cout << payload << "\n\n";

            std::string response = read();
            std::cout << "\n";
            std::cout << response << "\n";
            std::cout << "\n";
        }
};

} // namespace



