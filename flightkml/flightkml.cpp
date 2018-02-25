#include "flightkml.h"

#include <libxml++/libxml++.h>

#include <boost/date_time.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <ctime>
#include <cstring>
#include <iomanip>
#include <algorithm>

namespace flightkml {

Point::Point(boost::posix_time::ptime time, double latitude, double longitude, double altitude) :
    _time(time),
    _latitude(latitude),
    _longitude(longitude),
    _altitude(altitude)
{
}

namespace detail {

/** A latitude, longitude, and altitude */
struct LatLonAlt {
    double latitude;
    double longitude;
    double altitude;

    /**
     * Parses a latitude/longitude/altitude from a string containing three
     * space-separated numbers
     *
     * On success, returns 0 and writes to result
     * On failure, returns -1 and writes an error message to error
     */
    static int from_string(const Glib::ustring& s, LatLonAlt* result, std::string* error) {
        std::stringstream stream(s);
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
};

class FlightSaxParser : public xmlpp::SaxParser {
private:
    /** Parse warnings */
    std::vector<Glib::ustring> _warnings;
    /** Parse errors */
    std::vector<Glib::ustring> _errors;
    /** Parse fatal errors */
    std::vector<Glib::ustring> _fatal_errors;

    // Parsed data
    /** Latitude/longitude/altitude entries */
    std::vector<LatLonAlt> _lat_lon_alt;
    /** Time entries, in UTC */
    std::vector<boost::posix_time::ptime> _time;

    // Parsing state
    /** In a <gx:Track> element */
    bool _in_track;
    /** In a <when> element, expecting a date and time */
    bool _in_when;
    /** In a <gx:coord> element, expecting coordinates */
    bool _in_coord;

public:
    FlightSaxParser() = default;
    virtual ~FlightSaxParser() = default;

    const std::vector<Glib::ustring>& warnings() const {
        return _warnings;
    }
    const std::vector<Glib::ustring>& errors() const {
        return _errors;
    }
    const std::vector<Glib::ustring>& fatal_errors() const {
        return _fatal_errors;
    }

    const std::vector<LatLonAlt> lat_lon_alt() const {
        return _lat_lon_alt;
    }
    const std::vector<boost::posix_time::ptime> time() const {
        return _time;
    }

protected:
    virtual void on_start_document() override {
        // Initialize
        _warnings.clear();
        _errors.clear();
        _fatal_errors.clear();
        _lat_lon_alt.clear();
        _time.clear();
        _in_track = false;
        _in_when = false;
        _in_coord = false;
    }
    virtual void on_end_document() override {
        // Check lat/lon/alt and time matching
        if (_lat_lon_alt.size() != _time.size()) {
            std::stringstream err_stream;
            err_stream <<
                "Mismatch between lengths of latitude/longitude/altitude ("
                << _lat_lon_alt.size() << ") and time (" << _time.size() << ")";
            _errors.push_back(err_stream.str());
        }
    }
    virtual void on_start_element(const Glib::ustring& name, const AttributeList& attributes) override {
        if (name == "gx:Track") {
            _in_track = true;
        }
        if (name == "when") {
            _in_when = true;
        }
        if (name == "gx:coord") {
            _in_coord = true;
        }
    }
    virtual void on_end_element(const Glib::ustring& name) override {
        if (name == "gx:Track") {
            _in_track = false;
        }
        if (name == "when") {
            _in_when = false;
        }
        if (name == "gx:coord") {
            _in_coord = false;
        }
    }
    virtual void on_characters(const Glib::ustring& text) override {
        if (_in_track && _in_when) {

            boost::posix_time::ptime time;
            boost::posix_time::time_input_facet* tif = new boost::posix_time::time_input_facet;
            tif->set_iso_extended_format();
            std::istringstream iss(text);
            iss.imbue(std::locale(std::locale::classic(), tif));
            iss >> time;
            if (iss) {
                _time.push_back(time);
            } else {
                _errors.push_back("Invalid time format");
            }
        }
        if (_in_track && _in_coord) {
            detail::LatLonAlt lla;
            std::string error;
            const auto status = detail::LatLonAlt::from_string(text, &lla, &error);
            if (status == 0) {
                _lat_lon_alt.push_back(lla);
            } else {
                _errors.push_back(error);
            }
        }
    }
    virtual void on_warning(const Glib::ustring& text) override {
        _warnings.push_back(text);
    }
    virtual void on_error(const Glib::ustring& text) override {
        _errors.push_back(text);
    }
    virtual void on_fatal_error(const Glib::ustring& text) override {
        _fatal_errors.push_back(text);
    }
};

}

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

}
