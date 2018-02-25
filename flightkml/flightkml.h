
#include <string>
#include <vector>
#include <chrono>

namespace flightkml {

typedef std::chrono::system_clock::time_point time_point;



/**
 * The position of an aircraft during a flight
 */
class Point {
private:
    /** Time, UTC */
    time_point _time;
    /** Latitude, degrees */
    double _latitude;
    /** Longitude, degrees */
    double _longitude;
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
};

}
