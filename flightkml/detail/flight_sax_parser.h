#ifndef FLIGHTKML_DETAIL_FLIGHT_SAX_PARSER_H
#define FLIGHTKML_DETAIL_FLIGHT_SAX_PARSER_H

#include <libxml++/parsers/saxparser.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <vector>

#include "lat_lon_alt.h"

namespace flightkml {
namespace detail {

/**
 * A parser that parses KML files and extracts flight data
 */
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

    const std::vector<Glib::ustring>& warnings() const;
    const std::vector<Glib::ustring>& errors() const;
    const std::vector<Glib::ustring>& fatal_errors() const;

    const std::vector<LatLonAlt>& lat_lon_alt() const;
    const std::vector<boost::posix_time::ptime>& time() const;

protected:
    virtual void on_start_document() override;
    virtual void on_end_document() override;
    virtual void on_start_element(const Glib::ustring& name, const AttributeList& attributes) override;
    virtual void on_end_element(const Glib::ustring& name) override;
    virtual void on_characters(const Glib::ustring& text) override;
    virtual void on_warning(const Glib::ustring& text) override;
    virtual void on_error(const Glib::ustring& text) override;
    virtual void on_fatal_error(const Glib::ustring& text) override;
};

}
}

#endif
