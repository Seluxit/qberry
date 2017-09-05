#include "state.hpp"
#include "value.hpp"
#include "utils.hpp"
#include <string>
#include <iostream>
#include <thread>
#include "reactor.hpp"

namespace berry {
    
    State::State(const std::shared_ptr<Value>& parent, 
                 const std::string& uuid, 
                 const std::shared_ptr<Gpio>& pin) : parent_(parent), pin_(pin), id(uuid) 
    {
        
        std::string filename = this->location() + this->id + "/state.json";
        deserialize(filename, *this);

        // Watch if pin is for Reading "in" should be for reading, "out" for controlling
        if (pin_->direction() == Direction::in) {
           
            wstate_.set<State, &State::checkin>(this);       
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
    void State::checkin(ev::timer& w, int revents)
    {
        std::string value = pin_->value();
        if (this->data != value) {
            this->data = value; 
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
