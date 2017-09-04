#pragma once

#include <string>

namespace berry {

enum Direction { in, out };

class Gpio
{
	private:
		int number_; 
		Direction direction_;

        std::string export_path_ = "/sys/class/gpio/export";
        std::string unexport_path_ = "/sys/class/gpio/unexport";
        std::string root_path_ = "/sys/class/gpio/gpio";
    
        void export_pin(int number);
        void set_direction(Direction dir);
        void set_edge(const std::string& edge);
	public:
		Gpio(int x, Direction dir); 
		~Gpio();		

		std::string value();
		bool value(const std::string& val);		
    	std::string value_path();

		int number();
		Direction direction();
};

} // namespace
