#include "flight_sax_parser.h"
#include <boost/date_time/posix_time/posix_time.hpp>

namespace flightkml {
namespace detail {

const std::vector<Glib::ustring>& FlightSaxParser::warnings() const {
    return _warnings;
}
const std::vector<Glib::ustring>& FlightSaxParser::errors() const {
    return _errors;
}
const std::vector<Glib::ustring>& FlightSaxParser::fatal_errors() const {
    return _fatal_errors;
}

const std::vector<LatLonAlt>& FlightSaxParser::lat_lon_alt() const {
    return _lat_lon_alt;
}
const std::vector<boost::posix_time::ptime>& FlightSaxParser::time() const {
    return _time;
}

void FlightSaxParser::on_start_document() {
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
void FlightSaxParser::on_end_document() {
    // Check lat/lon/alt and time matching
    if (_lat_lon_alt.size() != _time.size()) {
        std::stringstream err_stream;
        err_stream <<
            "Mismatch between lengths of latitude/longitude/altitude ("
            << _lat_lon_alt.size() << ") and time (" << _time.size() << ")";
        _errors.push_back(err_stream.str());
    }
}
void FlightSaxParser::on_start_element(const Glib::ustring& name, const AttributeList& attributes) {
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
void FlightSaxParser::on_end_element(const Glib::ustring& name) {
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
void FlightSaxParser::on_characters(const Glib::ustring& text) {
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
void FlightSaxParser::on_warning(const Glib::ustring& text) {
    _warnings.push_back(text);
}
void FlightSaxParser::on_error(const Glib::ustring& text) {
    _errors.push_back(text);
}
void FlightSaxParser::on_fatal_error(const Glib::ustring& text) {
    _fatal_errors.push_back(text);
}

}
}
