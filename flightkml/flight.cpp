#include "flight.h"
#include "detail/flight_sax_parser.h"
#include "detail/lat_lon_alt.h"

#include <libxml++/libxml++.h>

#include <iostream>
#include <stdexcept>
#include <ctime>
#include <cstring>
#include <iomanip>
#include <algorithm>

namespace flightkml {

Flight::Flight(std::vector<Point>&& points) :
    _points(points)
{

}

Flight Flight::read_from_kml(const std::string& path) {
    detail::FlightSaxParser parser;
    parser.parse_file(path);

    for (const auto& warning : parser.warnings()) {
        std::cerr << "KML parse warning: " << warning << '\n';
    }
    for (const auto& error : parser.errors()) {
        std::cerr << "KML parse error: " << error << '\n';
    }
    for (const auto& error : parser.fatal_errors()) {
        std::cerr << "KML parse fatal error: " << error << '\n';
    }

    // Convert lat/lon/alt into Points
    const auto lla = parser.lat_lon_alt();
    const auto time = parser.time();
    const auto point_count = std::min(lla.size(), time.size());
    std::vector<Point> points;
    points.reserve(point_count);
    for (std::size_t i = 0; i < point_count; i++) {
        const auto point = Point(time[i], lla[i].latitude, lla[i].longitude, lla[i].altitude);
        points.emplace_back(std::move(point));
    }

    return Flight(std::move(points));
}

const std::vector<Point>& Flight::points() const {
    return _points;
}

boost::posix_time::ptime Flight::departure_time() const {
    if (_points.size() == 0) {
        return boost::posix_time::ptime();
    } else {
        return _points[0].time();
    }
}

}
