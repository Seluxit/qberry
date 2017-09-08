
Qberry - Raspbeery Pi application for controlling GPIO pins 
===================================================

Copyright 2017 Seluxit A/S.

Intro
-----------------------

QBerry is a C++ application for controlling GPIO pins. Before you run application run setup (interactive script 'generate_data.sh') to 
define which pins are need to be controlled, describe device and value information. All generated data will be save into
json format under 'network' directory. 


Installation of dependencies 
-----------------------

To build QBerry, the following tools/libraries are needed:

  * g++ - C++ compiler with c++11 support: g++4.8 and above.
  * [CMake][cmake-org] - cross platform tool for building Qplus  
  * libev - high performance event loop - event library.

[cmake-org]: https://cmake.org/

On Ubuntu, you can install them with:

    $ sudo apt-get install cmake libev-dev


## Build and usage

For building QBerry you need to change directory to `build`. 
Run './configure' for configuration and 'make' to build the QBerry. 
Once build is finished it should produce `qberry` executable. Executable will read 'network' directory structure with all
devices, values and states, wrap such data in json format and send to [data pipe][pipe] and [rest service][rest]. Those
services can be run on local Rastberry Pi or on a remote machine.
Application It also will listen on event from [data pipe][pipe] for controlling/updating states. 
Use [Postman][postman-com] or other REST clients to read/control device state.

[postman-com]: https://www.getpostman.com/
[pipe]: https://github.com/Seluxit/pi-rest/blob/master/pipepi.py
[rest]: https://github.com/Seluxit/pi-rest/blob/master/restpi.py

## License

See the `LICENSE` file for details. In summary, QBerry is licensed under the
MIT license, or public domain if desired and recognized in your jurisdiction.
