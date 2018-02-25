# Finds libkml ( https://github.com/google/libkml )

include(SelectLibraryConfigurations)
include(FindPackageHandleStandardArgs)

SET(KML_VALID_COMPONENTS
    base
    convenience
    dom
    engine
    regionator
    xsd
)

# Check that components are valid
# Select core by default (does not need to be selected manually)
SET(KML_CHOSEN_COMPONENTS "base")
IF(kml_FIND_COMPONENTS)
	FOREACH(component ${kml_FIND_COMPONENTS})
		LIST(FIND KML_VALID_COMPONENTS ${component} component_location)
		IF(${component_location} EQUAL -1)
			MESSAGE(FATAL_ERROR "\"${component}\" is not a valid libkml component.")
		ELSE()
			LIST(FIND KML_CHOSEN_COMPONENTS ${component} component_location)
			IF(${component_location} EQUAL -1)
				LIST(APPEND KML_CHOSEN_COMPONENTS ${component})
			ENDIF()
		ENDIF()
	ENDFOREACH()
ENDIF()

# Find the library for each component
SET(KML_LIBRARIES "")
SET(KML_FOUND TRUE)
FOREACH(component ${KML_CHOSEN_COMPONENTS})
	MESSAGE(STATUS "Looking for KML component ${component}")
	FIND_LIBRARY(KML_COMPONENT_LIBRARY
		kml${component}
		HINTS
		${KML_PATH}
		$ENV{LD_LIBRARY_PATH}
		$ENV{KML_PATH}
	)
	MESSAGE(STATUS "Component library: ${KML_COMPONENT_LIBRARY}")
	IF(${KML_COMPONENT_LIBRARY} STREQUAL KML_COMPONENT_LIBRARY-NOTFOUND)
		MESSAGE(WARNING "No library found for KML component ${component}")
		SET(KML_FOUND FALSE)
	ELSE()
		LIST(APPEND KML_LIBRARIES ${KML_COMPONENT_LIBRARY})
	ENDIF()
	UNSET(KML_COMPONENT_LIBRARY CACHE)
ENDFOREACH()

find_path(KML_INCLUDE_DIR kml/base/file.h)

find_package_handle_standard_args(kml DEFAULT_MSG KML_LIBRARIES KML_INCLUDE_DIR)
