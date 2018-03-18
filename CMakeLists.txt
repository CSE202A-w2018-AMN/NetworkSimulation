cmake_minimum_required(VERSION 3.1)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# flightkml library
add_subdirectory(flightkml)

set(CMAKE_CXX_STANDARD 11)

# Main package

# NS3
find_package(NS3 3.26 REQUIRED COMPONENTS core network applications mobility)
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
    src/util/bits.h
    src/util/bits.cpp
    src/util/print_container.h
    src/util/value_iterator.h
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
    src/network/olsr/olsr.h
    src/network/olsr/olsr.cpp
    src/network/olsr/header.h
    src/network/olsr/header.cpp
    src/network/olsr/message.h
    src/network/olsr/message.cpp
    src/network/olsr/neighbor_table.h
    src/network/olsr/neighbor_table.cpp
    src/network/olsr/multipoint_relay.h
    src/network/olsr/multipoint_relay.cpp
    src/network/olsr/mpr_table.h
    src/network/olsr/mpr_table.cpp
    src/network/olsr/topology_table.h
    src/network/olsr/topology_table.cpp
    src/network/olsr/routing_table.h
    src/network/olsr/routing_table.cpp
    src/network/olsr/routing_calc.h
    src/network/olsr/routing_calc.cpp
    src/recorder/record.h
    src/recorder/record.cpp
    src/recorder/session.h
    src/recorder/session.cpp
    src/recorder/session_recorder.h
    src/recorder/session_recorder.cpp
)

add_executable(${TARGET} ${SOURCES})
include_directories(${NS3_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR})
target_link_libraries(${TARGET} ${NS3_LIBRARIES} ${Boost_LIBRARIES} flightkml)
