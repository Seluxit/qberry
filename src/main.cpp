#include "configuration.hpp"
#include "path.hpp"
#include <iostream>
#include <cassert>
#include "reactor.hpp"
#include "network.hpp"
#include "utils.hpp"
#include "gpio.hpp"


int main()
{
    Conf_t conf = readConfiguration();
    berry::Path path(conf.QBERRY_PATH + "/network");
    assert(path.directory() != path.dirs_end()); 

    auto reactor = berry::Reactor::instance();
    reactor->init();

   


    auto& loop = reactor->defaultLoop();
    loop.run();
    std::cout << "end\n";
    return 0;
}


