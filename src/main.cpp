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
    
    if(path.directory() == path.dirs_end()) {
        std::cout << "Probably application do not have any data. Check 'network' directory\n";
        return -1;
    }
    
    std::string network_uuid = *path.directory();

    auto reactor = berry::Reactor::instance();
    reactor->init(conf, network_uuid);

   


    auto& loop = reactor->defaultLoop();
    loop.run();
    std::cout << "end\n";
    return 0;
}


