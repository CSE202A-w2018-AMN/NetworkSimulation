#ifndef FLIGHTKML_FLIGHT_H
#define FLIGHTKML_FLIGHT_H
#include <string>
#include <vector>

#include "point.h"

namespace flightkml {

/**
 * Information about a flight
 */
class Flight {
private:
    /** The points that define this flight */
    std::vector<Point> _points;

    /** Creates a Flight from a vector of points */
    Flight(std::vector<Point>&& points);
public:
    /**
     * Reads a flight from a Google Earth-compatible KML file
     * The FlightAware website provides such files for download.
     */
    static Flight read_from_kml(const std::string& path);

    /** Returns the points in this flight */
    const std::vector<Point>& points() const;

    /**
     * Returns the departure time of this flight
     *
     * If the flight has no points, a default-constructed ptime is returned.
     */
    boost::posix_time::ptime departure_time() const;

    /**
     * Returns the arrival time of this flight
     *
     * If the flight has no points, a default-constructed ptime is returned.
     */
     boost::posix_time::ptime arrival_time() const;
};

}

#endif
