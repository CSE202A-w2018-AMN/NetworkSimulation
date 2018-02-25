#ifndef FLIGHTKML_DETAIL_LAT_LON_ALT_H
#define FLIGHTKML_DETAIL_LAT_LON_ALT_H

#include <string>

namespace flightkml {
namespace detail {


/** A latitude, longitude, and altitude */
struct LatLonAlt {
    /** Latitude, degrees */
    double latitude;
    /** Longitude, degrees */
    double longitude;
    /** Altitude, meters */
    double altitude;

    /**
     * Parses a latitude/longitude/altitude from a string containing three
     * space-separated numbers
     *
     * On success, returns 0 and writes to result
     * On failure, returns -1 and writes an error message to error
     */
    static int from_string(const std::string& s, LatLonAlt* result, std::string* error);
};

}
}

#endif
