#include "state.hpp"
#include "value.hpp"
#include "utils.hpp"
#include <string>
#include <iostream>
#include <thread>
#include "reactor.hpp"

namespace berry {
    
    State::State(const Value* parent, const std::string& uuid) : parent_(parent), id(uuid) 
    {
        auto reactor = Reactor::instance();
        std::string root_path = reactor->configuration().QBERRY_PATH; 
       
        std::string filename = root_path + this->location() + this->id + "/state.json";
        deserialize(filename, *this);
        
        Direction dir = (this->type == "Report") ? Direction::in : Direction::out;
        
        // clean the pin state - unexport
        unexport_pin();

        try {
            pin_ = std::make_shared<berry::Gpio>(this->pin, dir); 
            // Watch if pin is for Reading "in" should be for reading, "out" for controlling
            if (pin_->direction() == Direction::in) {
                wstate_.set<State, &State::checkin>(this);       
                wstate_.start(.5, .5);
            }
        }
        catch(std::exception& e) {
            std::cerr << "\n";
            std::cerr << "Error: " << e.what() << "\n";
            std::cerr << "\n";
        }
    }

    std::string State::toJson() const
    {    
        json value;
        
        value[":type"] = "urn:seluxit:xml:bastard:state-1.1";
        value[":id"] = this->id;
        value["data"] = this->data;
        value["timestamp"] = timestampToDateTime(berry::timestamp());
        value["type"] = this->type;

        return value.dump();
    }

    void State::fromJson(const json& value)
    {
        this->id = value[":id"].get<std::string>() ;
        this->data = value["data"].get<std::string>(); 
        this->type = value["type"].get<std::string>();
        this->pin = value["pin"].get<int>();
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
            reactor->send("PUT", this);
        }
    }
    
    /**
     * Write
     */
    void State::set(const std::string& data)
    {
        if (pin_)
            pin_->value(data);
        else {
            std::cerr << "Can NOT set data to pin. Pin is invalid!\n";
        }
    }
    
    void State::unexport_pin()
    {
        if (pin_)
            pin_->unexport_pin();
    }
} // namespace 
