
set(FLIGHTKML_TARGET flightkml)
set(SOURCES flightkml.cpp flightkml.h)

set(CMAKE_CXX_STANDARD 11)

# Find libkml
find_package(xml++ 2.6 REQUIRED)
find_package(glibmm 2.4 REQUIRED)
find_package(glib 2.0 REQUIRED)
# Boost date/time
find_package(Boost REQUIRED COMPONENTS date_time)

add_library(${FLIGHTKML_TARGET} ${SOURCES})
target_link_libraries(${FLIGHTKML_TARGET} ${XML++_LIBRARIES} ${GLIBMM_LIBRARIES} ${GLIB_LIBRARIES} ${Boost_LIBRARIES})
include_directories(${XML++_INCLUDE_DIR} ${GLIBMM_INCLUDE_DIR} ${GLIB_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

add_subdirectory(flightkml_tests)
