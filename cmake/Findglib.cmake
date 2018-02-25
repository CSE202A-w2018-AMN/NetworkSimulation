# Finds glib

include(SelectLibraryConfigurations)
include(FindPackageHandleStandardArgs)

find_library(GLIB_LIBRARIES glib-${glib_FIND_VERSION})

MESSAGE("glib: looking for glib-${glib_FIND_VERSION}")

find_path(GLIB_INCLUDE_DIR glib.h
    HINTS
    /usr/include/glib-${glib_FIND_VERSION}
)

find_path(GLIB_CONFIG_INCLUDE_DIR glibconfig.h
    HINTS /usr/lib/x86_64-linux-gnu/glib-${glib_FIND_VERSION}/include
)
IF(${GLIB_CONFIG_INCLUDE_DIR} STREQUAL GLIB_CONFIG_INCLUDE_DIR-NOTFOUND)
    MESSAGE(FATAL_ERROR "glibconfig.h not found")
ENDIF()

MESSAGE("glib: libraries ${GLIB_LIBRARIES} path ${GLIB_INCLUDE_DIR}")

find_package_handle_standard_args(glib DEFAULT_MSG GLIB_LIBRARIES GLIB_INCLUDE_DIR)

# Add the config include path
set(GLIB_INCLUDE_DIR ${GLIB_INCLUDE_DIR} ${GLIB_CONFIG_INCLUDE_DIR})
