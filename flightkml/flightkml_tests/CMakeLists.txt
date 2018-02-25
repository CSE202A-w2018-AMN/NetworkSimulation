
# Test executable
set(TARGET flightkmltest)


add_executable(${TARGET} flightkml_test.cpp)
target_link_libraries(${TARGET} ${FLIGHTKML_TARGET} ${KML_LIBRARIES})
include_directories(${KML_INCLUDE_DIR})
include_directories(..)
