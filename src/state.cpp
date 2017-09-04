#include "state.hpp"
#include "value.hpp"
#include "utils.hpp"
#include <string>
#include <iostream>
#include <thread>
#include "reactor.hpp"
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <sys/ioctl.h>

namespace berry {
    
    State::State(const std::shared_ptr<Value>& parent, const std::string& uuid, const std::shared_ptr<Gpio>& pin) : parent_(parent), pin_(pin), id(uuid) 
    {
        // Watch if pin is for Reading "in" should be for reading, "out" for controling
        if (pin_->direction() == Direction::in) {
           
            wstate_.set<State, &State::callback>(this);       
            wstate_.start(.5, .5);
        }
    }

    std::string State::toJson() const
    {    
        json value;
        
        value[":type"] = "urn:seluxit:xml:bastard:state-1.1";
        value[":id"] = this->id;
        value["data"] = this->data;
        value["timestamp"] = this->timestamp;
        value["type"] = this->type;

        return value.dump();
    }

    void State::fromJson(const json& value)
    {
        this->id = value["id"].get<std::string>() ;
        this->data = value["data"].get<std::string>(); 
        this->timestamp = value["timestamp"].get<std::string>();
        this->type = value["type"].get<std::string>();
    }

    std::string State::location() const
    {
        return parent_->location() + parent_->id + "/state/"; 
    }

    /**
     * Read
     */
    void State::callback(ev::timer& w, int revents)
    {
        std::string val = pin_->value();
        if (intervalue_ != std::stoi(val)) {
            intervalue_ = std::stoi(val);
            this->data = val; 

            std::cout << "Value changed! " << intervalue_ << "\n";
            auto reactor = Reactor::instance();
            reactor->post(this);
        }
    }
    
    /**
     * Write
     */
    void State::set(const std::string& data)
    {
        pin_->value(data);
    }

} // namespace 
