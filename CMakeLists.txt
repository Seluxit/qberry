cmake_minimum_required(VERSION 2.8)

project(qberry)

# The version number.
set (MAJOR_VERSION 0)
set (MINOR_VERSION 1)
set (PATCH_VERSION 0)

message(STATUS "Current project dir: ${CMAKE_CURRENT_SOURCE_DIR}")

if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.conf")
    message(FATAL_ERROR "Can NOT find configuration file: ${PROJECT_NAME}.conf in path: ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

set (QBERRY_CONF    "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.conf")
set (QBERRY_PATH    "${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Found ${PROJECT_NAME} configuration file: ${QPLUS_CONF}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/configuration.hpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/configuration.hpp")


set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(CMAKE_CXX_FLAGS "-g --std=c++11 -Wno-psabi")

find_package(EV REQUIRED) 

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    )

link_directories(${CMAKE_BINARY_DIR}/lib)

add_executable(${PROJECT_NAME} 
    ${CMAKE_SOURCE_DIR}/src/state.cpp
    ${CMAKE_SOURCE_DIR}/src/value.cpp
    ${CMAKE_SOURCE_DIR}/src/device.cpp
    ${CMAKE_SOURCE_DIR}/src/network.cpp
    ${CMAKE_SOURCE_DIR}/src/reactor.cpp
    ${CMAKE_SOURCE_DIR}/src/gpio.cpp
    ${CMAKE_SOURCE_DIR}/src/path.cpp
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    )

target_link_libraries(${PROJECT_NAME} 
    ${LIBEV_LIBRARIES}
    pthread
    )

