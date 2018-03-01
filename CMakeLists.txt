cmake_minimum_required(VERSION 3.1)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# flightkml library
add_subdirectory(flightkml)

set(CMAKE_CXX_STANDARD 11)

# Main package

# NS3
find_package(NS3 3.26 REQUIRED COMPONENTS core network internet point-to-point applications mobility csma)
# Boost
find_package(Boost REQUIRED COMPONENTS filesystem)

# Enable logging in local code
add_definitions(-DNS3_LOG_ENABLE=1)

# Warnings
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
endif()

set(TARGET simulation)
set(SOURCES
    src/main.cpp
    src/flight_mobility.h
    src/flight_mobility.cpp
    src/flight_group.h
    src/flight_group.cpp
    src/flight_load.h
    src/flight_load.cpp
    src/address/icao_address.h
    src/address/icao_address.cpp
    src/device/mesh_net_device.h
    src/device/mesh_net_device.cpp
    src/header/mesh_header.h
    src/header/mesh_header.cpp
    src/application/adsb_sender.h
    src/application/adsb_sender.cpp
    src/application/adsb_sender_helper.h
    src/application/adsb_sender_helper.cpp
    src/ether/ether.h
    src/ether/ether.cpp
)

add_executable(${TARGET} ${SOURCES})
include_directories(${NS3_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR})
target_link_libraries(${TARGET} ${NS3_LIBRARIES} ${Boost_LIBRARIES} flightkml)
