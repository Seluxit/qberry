#pragma once

#include "json.hpp"
#include "date.hpp"
#include <exception>
#include <fstream>
#include <chrono>
#include "error.hpp"

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

    static std::string timestampToDateTime(const unsigned long long& timestamp) 
    {
        // time wihout miliseconds. 
        auto tp = timestamp ? 
            std::chrono::time_point<std::chrono::system_clock>(std::chrono::duration<unsigned long long>(timestamp / 1000 )) : 
            std::chrono::system_clock::now(); 

        auto dp = date::floor<date::days>(tp); // create day point
        auto ymd = date::year_month_day{dp};
        auto time = date::make_time(std::chrono::duration_cast<std::chrono::milliseconds>(tp-dp));
        
        std::stringstream mytime;
        mytime << ymd.year() << "-";
        mytime << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd.month()) << "-"; // cast in int
        mytime << std::setw(2) << std::setfill('0') << ymd.day() << "T";
        mytime << std::setw(2) << std::setfill('0') << time.hours().count() << ":";
        mytime << std::setw(2) << std::setfill('0') << time.minutes().count() << ":";
        mytime << std::setw(2) << std::setfill('0') << time.seconds().count() << "." << time.subseconds().count() << "Z";

        return mytime.str();
    }

    template<typename T>
    void serialize(const T& object, const std::string& filename)
    {
        std::ofstream os(filename.c_str(), std::ios::out);
        
        json object_json = object.toJson();
        os << object_json;
        os.close();
    }

    template<typename T>
    void deserialize(const std::string& path, T& object)
    {
        std::ifstream is(path.c_str()); 
        if (!is.good()) {
            throw error_t("Can't open configuration file: " + path);
        }
        
        try {
            json object_json;
            is >> object_json;
            is.close();
            object.fromJson(object_json);
        }
        catch(std::exception& e) {
            std::cerr << "Object UUID: " << object.id << " " << e.what() << std::endl;
        }
    }

} // namespace
