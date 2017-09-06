#pragma once

#include <string>
#include <vector>
#include <memory>
#include "error.hpp"
#include "value.hpp"
#include "json.hpp"

namespace berry {

using json = nlohmann::json;
class Network;

class Device {
    
    private:
        const Network* parent_;

    public:
        Device(const Network* parent, const std::string& uuid);
        
        std::string id;
        std::string name;
        std::string manufacturer;
        std::string product;
        std::string version;
        std::string serial;
        std::string protocol;
        std::string communication;
        std::string included;
    
        std::vector<std::shared_ptr<Value>> values;

        std::string location() const;
        std::string toJson() const; 
        void fromJson(const json& value);
};

} // namespace qplus
