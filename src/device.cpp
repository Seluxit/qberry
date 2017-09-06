#include "device.hpp"
#include "network.hpp"
#include "path.hpp"
#include "reactor.hpp"

namespace berry {
    
    Device::Device(const Network* parent, const std::string& uuid) : parent_(parent), id(uuid)
    {
        auto reactor = Reactor::instance();
        std::string root_path = reactor->configuration().QBERRY_PATH; 
        std::string filename = root_path + this->location() + this->id + "/device.json";
        deserialize(filename, *this);
        
        // check if there is a values
        Path path(root_path + this->location() + this->id + "/value");
        while (path.directory() != path.dirs_end()) {
            
            auto value = std::make_shared<Value>(this, *path.directory());
            reactor->values.push_back(value);
            path.directory()++;
        }
    }
    
    std::string Device::toJson() const
    {
        json value;

        value[":type"] = "urn:seluxit:xml:bastard:device-1.1";
        
        value[":id"] = this->id;
        value["name"] = this->name;
        value["manufacturer"] = this->manufacturer;
        value["product"] = this->product;
        value["version"] = this->version;
        value["serial"] = this->serial;
        value["protocol"] = this->protocol;
        value["communication"] = this->communication;
        value["included"] = this->included;
        
        return value.dump();
    }

    void Device::fromJson(const json& value)
    {
        this->id = value.at(":id").get<std::string>();
        this->name = value.at("name").get<std::string>();
        this->manufacturer = value.at("manufacturer").get<std::string>();
        this->product = value.at("product").get<std::string>();
        this->version = value["version"].get<std::string>();
        this->serial = value["serial"].get<std::string>();
        this->protocol = value["protocol"].get<std::string>();
        this->communication = value["communication"].get<std::string>();
        this->included = value["included"].get<std::string>();
    }

    std::string Device::location() const
    {
        return parent_->location() + parent_->id + "/device/"; 
    }

}
