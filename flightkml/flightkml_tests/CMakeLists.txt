
# Test executable
set(TARGET flightkmltest)


add_executable(${TARGET} flightkml_test.cpp)
target_link_libraries(${TARGET} ${FLIGHTKML_TARGET} ${XML++_LIBRARIES})
include_directories(${XML++_INCLUDE_DIR})
include_directories(..)
