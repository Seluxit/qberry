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
    
        std::string location() const;

        // ----
        std::unique_ptr<Device> device_;
        std::string id; 
        const std::string name = "Seluxit fatek";
};


} // namespace qplus
