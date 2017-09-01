#include "state.hpp"
#include "value.hpp"
#include "utils.hpp"
#include <string>
#include <iostream>
#include <thread>
#include "reactor.hpp"

namespace berry {

    State::State(const std::shared_ptr<Value>& parent, const std::string& uuid, const std::shared_ptr<Gpio>& pin) : parent_(parent), pin_(pin), id(uuid) 
    {
        // Watch if pin is for Reading
        if (pin_->direction() == Direction::out) {
            wstate_.set<State, &State::callback>(this);       
            wstate_.start(pin_->value_path().c_str(), 0.);
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
    void State::callback(ev::stat& w, int revents)
    {
        std::cout << "Event - file: " << pin_->value_path()  << " has been changed!\n";
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        this->data = pin_->value();

        auto reactor = Reactor::instance();
        reactor->post(this);
    }
    
    /**
     * Write
     */
    void State::set(const std::string& data)
    {
        pin_->value(data);
    }

} // namespace 
