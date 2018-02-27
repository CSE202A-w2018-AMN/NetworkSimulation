#ifndef FLIGHT_MOBILITY_H
#define FLIGHT_MOBILITY_H

#include <ns3/waypoint-mobility-model.h>
#include <flightkml/flight.h>

/**
 * Fills a WaypointMobilityModel with waypoints from a flight
 *
 * @param flight the flight to get waypoints from
 * @param epoch the real-world time that will correspond to zero simulation time.
 * The time of each flight waypoint must be equal to or after the epoch time.
 * @param model the model to configure 
 */
void fill_flight_waypoints(const flightkml::Flight& flight, const boost::posix_time::ptime& epoch, ns3::WaypointMobilityModel* model);

#endif
