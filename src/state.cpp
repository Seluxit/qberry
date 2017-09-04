#include "state.hpp"
#include "value.hpp"
#include "utils.hpp"
#include <string>
#include <iostream>
#include <thread>
#include "reactor.hpp"
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

namespace berry {
    
    int State::wait_interrupt(unsigned int* val)
    {
        int fd; 
        short revents;
        struct pollfd pfd;

        fd = open(pin_->value_path().c_str(), O_RDONLY | O_NONBLOCK);
        pfd.fd = fd;
        pfd.events = POLLPRI | POLLERR;
        
        puts("loop");
        int x = poll(&pfd, 1, -1);
        revents = pfd.revents;

        if (revents & POLLPRI) {
            // If no error, do a dummy read to clear the interrupt
            //	A one character read appears to be enough.
            uint8_t c ;
            if (x > 0) {
                lseek (fd, 0, SEEK_SET) ;	// Rewind
                (void)read (fd, &c, 1) ;	// Read & clear
                if (c != '0') *val = 1;
                else *val = 0;
            }
        }
        return x;
    }

    void State::interrupt_handler()
    {
        for (;;) {
            if (wait_interrupt(&intervalue_) > 0)  {
                // read the value from file 
                std::cout << "Interrup occur!!\n";
                if (wstate_.is_pending() == false) { 
                    printf("Activate ev_async watcher\n");
                    wstate_.send();
                }
            }
        }
    }


    State::State(const std::shared_ptr<Value>& parent, const std::string& uuid, const std::shared_ptr<Gpio>& pin) : parent_(parent), pin_(pin), id(uuid) 
    {
        // Watch if pin is for Reading "in" should be for reading, "out" for controling
        if (pin_->direction() == Direction::in) {

            // 
            wstate_.set<State, &State::callback>(this);       
            wstate_.start();
            
            std::thread twatcher(&State::interrupt_handler, this);
            twatcher.detach();
        }
    }

    std::string State::toJson() const
    {    
        json value;
        
        value[":type"] = "urn:seluxit:xml:bastard:state-1.1";
        value[":id"] = this->id;
        value["data"] = this->data;
        value["timestamp"] = this->timestamp;
        value["type"] = this->type;

        return value.dump();
    }

    void State::fromJson(const json& value)
    {
        this->id = value["id"].get<std::string>() ;
        this->data = value["data"].get<std::string>(); 
        this->timestamp = value["timestamp"].get<std::string>();
        this->type = value["type"].get<std::string>();
    }

    std::string State::location() const
    {
        return parent_->location() + parent_->id + "/state/"; 
    }

    /**
     * Read
     */
    void State::callback(ev::async& w, int revents)
    {
        std::cout << "Event - file: " << pin_->value_path()  << " has been changed!\n";
        std::cout << "Event - file: " << intervalue_  << " has been changed!\n";
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        this->data = pin_->value();

        auto reactor = Reactor::instance();
        reactor->post(this);
    }
    
    /**
     * Write
     */
    void State::set(const std::string& data)
    {
        pin_->value(data);
    }

} // namespace 
