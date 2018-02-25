# From https://github.com/raedwulf/nssim/blob/master/cmake/FindNS3.cmake
# Author unknown

INCLUDE(SelectLibraryConfigurations)
INCLUDE(FindPackageHandleStandardArgs)

# List of the valid ns3 components.
SET(NS3_VALID_COMPONENTS
	antenna
	aodv
	applications
	bridge
	config-store
	core
	csma-layout
	csma
	dsdv
	dsr
	energy
	fd-net-device
	flow-monitor
	internet-apps
	internet
	lr-wpan
	lte
	mesh
	mobility
	mpi
	netanim
	network
	nix-vector-routing
	olsr
	point-to-point-layout
	point-to-point
	propagation
	sixlowpan
	spectrum
	stats
	tap-bridge
	test
	topology-read
	traffic-control
	uan
	virtual-net-device
	visualizer
	wave
	wifi
	wimax
)

# Check that components are valid
# Select core by default (does not need to be selected manually)
SET(NS3_CHOSEN_COMPONENTS "core")
IF(NS3_FIND_COMPONENTS)
	FOREACH(component ${NS3_FIND_COMPONENTS})
		LIST(FIND NS3_VALID_COMPONENTS ${component} component_location)
		IF(${component_location} EQUAL -1)
			MESSAGE(FATAL_ERROR "\"${component}\" is not a valid NS3 component.")
		ELSE()
			LIST(FIND NS3_CHOSEN_COMPONENTS ${component} component_location)
			IF(${component_location} EQUAL -1)
				LIST(APPEND NS3_CHOSEN_COMPONENTS ${component})
			ENDIF()
		ENDIF()
	ENDFOREACH()
ENDIF()

# Find the library for each component
SET(NS3_LIBRARIES "")
SET(NS3_FOUND TRUE)
FOREACH(component ${NS3_CHOSEN_COMPONENTS})
	MESSAGE(STATUS "Looking for NS3 component ${component}")
	FIND_LIBRARY(NS3_COMPONENT_LIBRARY
		NAMES ns3-${component} ns${NS3_FIND_VERSION}-${component}
		HINTS
		${NS3_PATH}
		$ENV{LD_LIBRARY_PATH}
		$ENV{NS3_PATH}
	)
	MESSAGE(STATUS "Component library: ${NS3_COMPONENT_LIBRARY}")
	IF(${NS3_COMPONENT_LIBRARY} STREQUAL NS3_COMPONENT_LIBRARY-NOTFOUND)
		MESSAGE(WARNING "No library found for NS3 component ${component} (tried ns3-${component} and ns${NS3_FIND_VERSION}-${component})")
		SET(NS3_FOUND FALSE)
	ELSE()
		LIST(APPEND NS3_LIBRARIES ${NS3_COMPONENT_LIBRARY})
	ENDIF()
	UNSET(NS3_COMPONENT_LIBRARY CACHE)
ENDFOREACH()

set(NS3_VERSION_SPECIFIC_INCLUDE_PATH /usr/include/ns${NS3_FIND_VERSION})

# Find the include dir for ns3.
FIND_PATH(NS3_INCLUDE_DIR
	NAME ns3/core-module.h
	HINTS
	${NS3_PATH}
	$ENV{NS3_PATH}
	PATH_SUFFIXES include ns3/include
	PATHS
	/opt
	/opt/local
	/opt/csw
	/sw
	${NS3_VERSION_SPECIFIC_INCLUDE_PATH}
	/usr
)


FIND_PACKAGE_HANDLE_STANDARD_ARGS(NS3 DEFAULT_MSG NS3_LIBRARIES NS3_INCLUDE_DIR)
MARK_AS_ADVANCED(NS3_LIBRARIES NS3_INCLUDE_DIR)
