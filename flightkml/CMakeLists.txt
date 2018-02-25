cmake_minimum_required(VERSION 3.1)

set(FLIGHTKML_TARGET flightkml)
set(SOURCES flightkml.cpp flightkml.h)

set(CMAKE_CXX_STANDARD 11)

# Find libkml
find_package(kml REQUIRED COMPONENTS base dom)

add_library(${FLIGHTKML_TARGET} ${SOURCES})
target_link_libraries(${FLIGHTKML_TARGET} ${KML_LIBRARIES})
include_directories(${KML_INCLUDE_DIR})

add_subdirectory(flightkml_tests)
