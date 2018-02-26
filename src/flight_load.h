#ifndef FLIGHT_LOAD_H
#define FLIGHT_LOAD_H
#include <string>
#include "flight_group.h"

/**
 * Loads flights from all KML files in the directory at the provided path
 * and returns them as a group
 */
FlightGroup load_flights(const std::string& directory);

#endif
