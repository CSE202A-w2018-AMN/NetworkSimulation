cmake_minimum_required(VERSION 3.1)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# flightkml library
add_subdirectory(flightkml)

set(CMAKE_CXX_STANDARD 11)

# Main package

# NS3
find_package(NS3 3.26 REQUIRED COMPONENTS core network internet point-to-point applications mobility)

find_package(Boost REQUIRED COMPONENTS filesystem)

set(TARGET simulation)
set(SOURCES
    src/main.cpp
    src/flight_mobility.h
    src/flight_mobility.cpp
    src/flight_group.h
    src/flight_group.cpp
    src/flight_load.h
    src/flight_load.cpp
)

add_executable(${TARGET} ${SOURCES})
include_directories(${NS3_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} .)
target_link_libraries(${TARGET} ${NS3_LIBRARIES} ${Boost_LIBRARIES} flightkml)