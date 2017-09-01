#include "network.hpp"
#include "device.hpp"
#include "json.hpp" 
#include <fstream>
#include <iostream>
#include "error.hpp"


namespace berry {
    
    using json = nlohmann::json;

    Network::Network(const std::string& uuid) : id(uuid)
    {

    }
        
    std::string Network::location() const
    {
        return "/network/"; 
    }

    std::string Network::toJson() const
    {
        json value;
        value[":type"] = "urn:seluxit:xml:bastard:network-1.1";
        value["name"] = this->name;
        value[":id"] = this->id;

        return value.dump();
    }

} // namespace qplus
