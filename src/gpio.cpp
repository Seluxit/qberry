#include "gpio.hpp"
#include <fstream>
#include <iostream>
#include "error.hpp"
#include <unistd.h>
#include <fcntl.h>

namespace berry {

    Gpio::Gpio(int number, Direction dir) : number_(number), direction_(dir)
    {
        export_pin(number_);
        set_direction(direction_);
    }
    
    Gpio::~Gpio()
    {
        std::string pin_number = std::to_string(number_);

        int fd = open(unexport_path_.c_str(), O_WRONLY);
        if (fd > 0) {
            write(fd, pin_number.c_str(), pin_number.size());
            close(fd);
        }
    }
    
    void Gpio::export_pin(int number) 
    {
        int fd = open(export_path_.c_str(), O_WRONLY);
        if (fd < 0) {
            throw error_t("Can NOT open file: " + export_path_);
        }

        std::string pin_number = std::to_string(number);
        write(fd, pin_number.c_str(), pin_number.size());
        close(fd);
    }
    
    void Gpio::set_direction(Direction dir)
    {
        std::string direction_path = root_path_ + std::to_string(number_) + "/direction";
        int fd = open(direction_path.c_str(), O_WRONLY);
        if (fd < 0) {
           throw error_t("Can NOT open file: " + direction_path); 
        }

        std::string direction = (Direction::in == dir ? "in" : "out");
        write(fd, direction.c_str(), direction.size());
        close(fd);
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
