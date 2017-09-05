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
        std::shared_ptr<Network> parent_;

    public:
        Device(const std::shared_ptr<Network>& parent, const std::string& uuid);
        Device(Network* parent);
        
        std::string id = "164aa29b-d75a-4544-aa29-b4640086b71a";
        std::string name = "Fatek device";
        std::string manufacturer;
        std::string product;
        std::string version;
        std::string serial;
        std::string protocol;
        std::string communication;
        int included = 1;
    
        std::vector<std::shared_ptr<Value>> values;

        std::string location() const;
        std::string toJson() const; 
        void fromJson(const json& value);
};

} // namespace qplus
