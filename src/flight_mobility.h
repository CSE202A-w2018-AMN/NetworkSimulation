#ifndef FLIGHT_MOBILITY_H
#define FLIGHT_MOBILITY_H

#include <ns3/waypoint-mobility-model.h>
#include <flightkml/flight.h>

/**
 * Creates a WaypointMobilityModel for a flight
 */
ns3::WaypointMobilityModel create_flight_mobility_model(const flightkml::Flight& flight);

#endif
