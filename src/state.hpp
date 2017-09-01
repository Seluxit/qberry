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
        std::shared_ptr<Value> parent_;
        
        std::shared_ptr<Gpio> pin_;
        ev::stat wstate_;
        void callback(ev::stat& w, int revents);

    public:
        State(const std::shared_ptr<Value>& parent, const std::string& uuid, const std::shared_ptr<Gpio>& pin);
        
        // ------
        std::string id;
        std::string type;
        std::string timestamp;
        std::string data;
        
        std::string location() const;
        std::string toJson() const;
        void fromJson(const json& value);

        void set(const std::string& data);
};

} // namespace


