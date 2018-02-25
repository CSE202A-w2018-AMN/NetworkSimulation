
#include <string>
#include <vector>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace flightkml {

/**
 * The position of an aircraft during a flight
 */
class Point {
private:
    /** Time, UTC */
    boost::posix_time::ptime _time;
    /** Latitude, degrees */
    double _latitude;
    /** Longitude, degrees */
    double _longitude;
    /** Altitude above mean sea level, meters */
    double _altitude;
public:
    Point(boost::posix_time::ptime time, double latitude, double longitude, double altitude);
};

/**
 * Information about a flight
 */
class Flight {
private:
    /** The points that define this flight */
    std::vector<Point> _points;

    Flight(std::vector<Point>&& points);
public:
    /**
     * Reads a flight from a Google Earth-compatible KML file
     * The FlightAware website provides such files for download.
     */
    static Flight read_from_kml(const std::string& path);

    /** Returns the points in this flight */
    const std::vector<Point>& points() const;
};

}
