#pragma once

#include "json.hpp"
#include <exception>
#include <chrono>

namespace berry {

typedef std::chrono::system_clock clk;
using json = nlohmann::json;
    
static std::string jsonRpc(const std::string& method, const std::string& ajson, const std::string& url)
{
    try {
        auto data = json::parse(ajson);
        json params;
        params["url"] = url;
        params["data"] = data;

        json value;
        value["jsonrpc"] = "2.0";
        value["id"] = "996600";
        value["method"] = method;
        value["params"] = params;

        return value.dump();
    }
    catch(std::exception& e) {
        
        std::cout << "Error: " << e.what() << "\n";
        return "";
    }
}

inline unsigned long long timestamp() 
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(clk::now().time_since_epoch()).count();
}


} // namespace
