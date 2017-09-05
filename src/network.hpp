#pragma once

#include <string>
#include <vector>
#include <memory>
#include "device.hpp"

namespace berry {

class Network {
    
    private:

    public:
        Network(const std::string& uuid);
        std::string toJson() const;
        void fromJson(const json& value);
    
        std::string location() const;

        std::string id; 
        const std::string name;
};


} // namespace qplus
