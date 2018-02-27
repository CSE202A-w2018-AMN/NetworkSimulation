#ifndef FLIGHT_MOBILITY_H
#define FLIGHT_MOBILITY_H

#include <ns3/waypoint-mobility-model.h>
#include <flightkml/flight.h>

/**
 * Fills a WaypointMobilityModel with waypoints from a flight
 */
void fill_flight_waypoints(const flightkml::Flight& flight, ns3::WaypointMobilityModel* model);

#endif
