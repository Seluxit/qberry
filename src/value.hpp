#pragma once

#include <string>
#include <memory>
#include "state.hpp"
#include "json.hpp"

namespace berry {
    
using json = nlohmann::json;
class Device;

class Value {

    private:
        const Device* parent_;

    public:
        Value(const Device* parent, const std::string& uuid);
        
        // ------
        std::string id;
        std::string name;
        std::string permission;
        std::string type;
        double min;
        double max;
        double step;
        
        std::string location() const;
        std::string toJson() const;
        void fromJson(const json& value);
};


} // namespace qplus
