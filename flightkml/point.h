#ifndef FLIGHTKML_POINT_H
#define FLIGHTKML_POINT_H
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
    /**
     * Creates a point with the provided time, latitude, longitude, and altitude
     */
    Point(boost::posix_time::ptime time, double latitude, double longitude, double altitude);

    inline double latitude() const {
        return _latitude;
    }
    inline double longitude() const {
        return _longitude;
    }
    inline double altitude() const {
        return _altitude;
    }
    /** Returns the time of this point */
    inline boost::posix_time::ptime time() const {
        return _time;
    }
};

}

#endif
