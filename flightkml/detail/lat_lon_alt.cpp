#include "lat_lon_alt.h"
#include <sstream>

namespace flightkml {
namespace detail {

int LatLonAlt::from_string(const std::string& s, LatLonAlt* result, std::string* error) {
    std::istringstream stream(s);
    stream >> result->latitude;
    if (!stream) {
        *error = "Invalid latitude";
        return -1;
    }
    stream >> result->longitude;
    if (!stream) {
        *error = "Invalid longitude";
        return -1;
    }
    stream >> result->altitude;
    if (!stream) {
        *error = "Invalid altitude";
        return -1;
    }
    return 0;
}

}
}
