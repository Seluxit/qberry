#include "gpio.hpp"
#include <fstream>
#include <iostream>
#include "error.hpp"

namespace berry {

    Gpio::Gpio(int number, Direction dir) : number_(number), direction_(dir)
    {
        std::ofstream numstream;
        numstream.open(export_path_.c_str(), std::ios::out);
        if (!numstream) {
            throw error_t("Can NOT open file: " + export_path_);
        }        
        
        numstream << std::to_string(number);        
        numstream.close();        
        
        std::ofstream dirstream;
        std::string direction_path = root_path_ + std::to_string(number_) + "/direction";
        dirstream.open(direction_path.c_str(), std::ios::out); 
 
        if (!dirstream) {
            throw error_t("Can NOT open file: " + direction_path);
        }           
        
        dirstream << (Direction::in == direction_ ? "in" : "out"); 
        dirstream.close();        
    }
    
    Gpio::~Gpio()
    {
        std::ofstream unstream(unexport_path_.c_str(), std::ios::out);
        if (unstream) {
            unstream << std::to_string(number_);
            unstream.close();
        }
    }

    bool Gpio::value(const std::string& val)
    {
        std::ofstream valstream(value_path().c_str(), std::ios::out);
        if (!valstream) {
            std::cerr << "Error: can NOT set value: " << val << std::endl;
            return false;
        } 
        
        valstream << val;
        valstream.close();       

        return true;        
    }

    std::string Gpio::value()
    {
        std::ifstream valstream(value_path().c_str());
        if (!valstream) {
            std::cerr << "Error: can NOT open file: " << value_path() << std::endl;
            return "";
        } 
        
        std::string val;
        valstream >> val; 
        
        valstream.close();
        return val;
    }    
 
    std::string Gpio::value_path()
    {
        return root_path_ + std::to_string(number_) + "/value";
    }    

    int Gpio::number()
    {
        return number_;
    }
        
    Direction Gpio::direction()
    {
        return direction_;
    }

}  // namespace 
