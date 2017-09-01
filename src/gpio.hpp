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
