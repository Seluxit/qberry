#include "value.hpp"
#include "device.hpp"
#include <string>
#include <iostream>
#include "utils.hpp"
#include "path.hpp"
#include "reactor.hpp"

namespace berry {

    Value::Value(const Device* parent, const std::string& uuid) : parent_(parent), id(uuid)
    {
        auto reactor = Reactor::instance();
        std::string root_path = reactor->configuration().QBERRY_PATH; 

        std::string filename = root_path + this->location() + this->id + "/value.json";
        deserialize(filename, *this);

        // check if there is a values
        Path path(root_path + this->location() + this->id + "/state");
        while (path.directory() != path.dirs_end()) {
            
            auto state = std::make_shared<State>(this, *path.directory());
            reactor->states.push_back(state);
            path.directory()++;
        }
    }

    std::string Value::toJson() const
    {    
        json value;
        
        value[":type"] = "urn:seluxit:xml:bastard:value-1.1";
        value[":id"] = this->id;
        value["name"] = this->name;
        value["permission"] = this->permission;
        value["type"] = this->type;

        json number;
        number["min"] = std::to_string(this->min);
        number["max"] = std::to_string(this->max);
        number["step"] = std::to_string(this->step);
        value["number"] = number;

        return value.dump();
    }

    void Value::fromJson(const json& value)
    {
        this->id = value[":id"].get<std::string>() ;
        this->name = value["name"].get<std::string>(); 
        this->permission = value["permission"].get<std::string>();
        this->type = value["type"].get<std::string>();

        this->min = value["min"].get<double>();
        this->max = value["max"].get<double>();
        this->step = value["step"].get<double>();
    }

    std::string Value::location() const
    {
        return parent_->location() + parent_->id + "/value/"; 
    }

} // namespace 
