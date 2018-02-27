#include "flight_mobility.h"
#include <ns3/geographic-positions.h>
#include <boost/date_time/posix_time/conversion.hpp>
#include <cassert>

void fill_flight_waypoints(const flightkml::Flight& flight, const boost::posix_time::ptime& epoch, ns3::WaypointMobilityModel* model) {
    // Clear
    model->EndMobility();
    long prev_seconds_since_epoch = 0;
    for (const auto& point : flight.points()) {
        // Convert latitude/longitude/altitude to earth-centered, earth-fixed
        const auto ecef_position = ns3::GeographicPositions::GeographicToCartesianCoordinates(
            point.latitude(),
            point.longitude(),
            point.altitude(),
            ns3::GeographicPositions::WGS84);
        // Convert into time relative to epoch
        const auto since_epoch = point.time() - epoch;
        const auto seconds_since_epoch = since_epoch.total_seconds();
        assert(seconds_since_epoch >= 0);
        // Ignore points with the same time
        if (seconds_since_epoch != prev_seconds_since_epoch) {
            const auto waypoint_time = ns3::Seconds(seconds_since_epoch);
            const auto waypoint = ns3::Waypoint(waypoint_time, ecef_position);
            model->AddWaypoint(waypoint);
        }
        prev_seconds_since_epoch = seconds_since_epoch;
    }
}
