#include "point.h"

namespace flightkml {

Point::Point(boost::posix_time::ptime time, double latitude, double longitude, double altitude) :
    _time(time),
    _latitude(latitude),
    _longitude(longitude),
    _altitude(altitude)
{
}

}
