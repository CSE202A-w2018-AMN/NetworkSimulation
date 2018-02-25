# Finds libxml++

include(SelectLibraryConfigurations)
include(FindPackageHandleStandardArgs)

find_library(XML++_LIBRARIES xml++-${xml++_FIND_VERSION})

find_path(XML++_INCLUDE_DIR libxml++/libxml++.h
    HINTS
    /usr/include/libxml++-${xml++_FIND_VERSION}
)

find_path(XML++_CONFIG_INCLUDE_DIR libxml++config.h
    HINTS /usr/lib/x86_64-linux-gnu/libxml++-${xml++_FIND_VERSION}/include
)
IF(${XML++_CONFIG_INCLUDE_DIR} STREQUAL XML++_CONFIG_INCLUDE_DIR-NOTFOUND)
    MESSAGE(FATAL_ERROR "libxml++config.h not found")
ENDIF()

find_package_handle_standard_args(xml++ DEFAULT_MSG XML++_LIBRARIES XML++_INCLUDE_DIR)

# Add the config include path
set(XML++_INCLUDE_DIR ${XML++_INCLUDE_DIR} ${XML++_CONFIG_INCLUDE_DIR})
