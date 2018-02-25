#include "flight_mobility.h"
#include <ns3/geographic-positions.h>
#include <boost/date_time/posix_time/conversion.hpp>

ns3::WaypointMobilityModel create_flight_mobility_model(const flightkml::Flight& flight) {
    auto model = ns3::WaypointMobilityModel();

    for (const auto& point : flight.points()) {
        // Convert latitude/longitude/altitude to earth-centered, earth-fixed
        const auto ecef_position = ns3::GeographicPositions::GeographicToCartesianCoordinates(
            point.latitude(),
            point.longitude(),
            point.altitude(),
            ns3::GeographicPositions::WGS84);
        // Convert into time relative to epoch
        const auto seconds_since_epoch = boost::posix_time::to_time_t(point.time());
        const auto waypoint_time = ns3::Seconds(seconds_since_epoch);
        const auto waypoint = ns3::Waypoint(waypoint_time, ecef_position);
        model.AddWaypoint(waypoint);
    }

    return model;
}
