#include <iostream>
#include "reactor.hpp"
#include "network.hpp"
#include "utils.hpp"
#include "gpio.hpp"


int main()
{

    auto reactor = berry::Reactor::instance();
    reactor->init();

   


    auto& loop = reactor->defaultLoop();
    loop.run();
    std::cout << "end\n";
    return 0;
}


