#include "network.hpp"
#include "device.hpp"
#include "json.hpp" 
#include <fstream>
#include <iostream>
#include "error.hpp"
#include "path.hpp"
#include "reactor.hpp"

namespace berry {
    
    using json = nlohmann::json;

    Network::Network(const std::string& uuid) : id(uuid)
    {
        auto reactor = Reactor::instance();
        Path path(reactor->configuration().QBERRY_PATH + this->location() + this->id + "/device");

        while (path.directory() != path.dirs_end()) {
            auto device = std::make_shared<Device>(this, *path.directory());
            reactor->devices.push_back(device);
            path.directory()++;
        }
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

    void Network::fromJson(const json& value)
    {

    }
} // namespace qplus
