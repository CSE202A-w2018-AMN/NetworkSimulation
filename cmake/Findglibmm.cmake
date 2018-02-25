# Finds glibmm

include(SelectLibraryConfigurations)
include(FindPackageHandleStandardArgs)

find_library(GLIBMM_LIBRARIES glibmm-${glibmm_FIND_VERSION})

MESSAGE("glibmm: looking for glibmm-${glibmm_FIND_VERSION}")

find_path(GLIBMM_INCLUDE_DIR glibmm.h
    HINTS
    /usr/include/glibmm-${glibmm_FIND_VERSION}
)

find_path(GLIBMM_CONFIG_INCLUDE_DIR glibmmconfig.h
    HINTS /usr/lib/x86_64-linux-gnu/glibmm-${glibmm_FIND_VERSION}/include
)
IF(${GLIBMM_CONFIG_INCLUDE_DIR} STREQUAL GLIBMM_CONFIG_INCLUDE_DIR-NOTFOUND)
    MESSAGE(FATAL_ERROR "glibmmconfig.h not found")
ENDIF()

MESSAGE("glibmm: libraries ${GLIBMM_LIBRARIES} path ${GLIBMM_INCLUDE_DIR}")

find_package_handle_standard_args(glibmm DEFAULT_MSG GLIBMM_LIBRARIES GLIBMM_INCLUDE_DIR)

# Add the config include path
set(GLIBMM_INCLUDE_DIR ${GLIBMM_INCLUDE_DIR} ${GLIBMM_CONFIG_INCLUDE_DIR})
