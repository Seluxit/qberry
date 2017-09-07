#pragma once

#include <string>
#include <ev++.h>
#include "json.hpp" 
#include "gpio.hpp"

namespace berry {

using json = nlohmann::json;    
class Value;

class State {

    private:
        const Value* parent_;
        std::shared_ptr<Gpio> pin_;
        
        ev::timer wstate_;
        void checkin(ev::timer& w, int revents);

    public:
        State(const Value* parent, const std::string& uuid);
        
        // ------
        std::string id;
        std::string type;
        std::string timestamp;
        std::string data;
        int pin;
        
        std::string location() const;
        std::string toJson() const;
        void fromJson(const json& value);

        void set(const std::string& data);
        void unexport_pin();
};

} // namespace


