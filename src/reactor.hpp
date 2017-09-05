#pragma once

#include "gpio.hpp"
#include <ev++.h>
#include <memory>
#include <iostream>
#include "utils.hpp"

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


        ev::io wconnection_;
        ev::default_loop loop_;
        
        int fdescriptor_ = 0;
		std::string hostname_ = "localhost";		
		int port_ = 21004;

        void socket_callback(ev::io& w, int revents);
    	int create_socket();
        void load_elements();

    public:
        static Reactor* instance();
        void init();
        
        std::shared_ptr<Network> network;
        std::vector<std::shared_ptr<Device>> devices;
        std::vector<std::shared_ptr<Value>> values;
        std::vector<std::shared_ptr<State>> states;

        std::string read() const;
        void write(const std::string& msg) const;
        ev::default_loop& defaultLoop();

        template<typename T>
        void post(const T& object)
        {
            std::string payload = jsonRpc("POST", object->toJson(), object->location());
            write(payload);            
            
            std::cout << payload << "\n\n";

            std::string response = read();
            std::cout << "\n";
            std::cout << response << "\n";
            std::cout << "\n";
        }

};

} // namespace



