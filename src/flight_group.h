#ifndef FLIGHT_GROUP_H
#define FLIGHT_GROUP_H

#include <vector>
#include <flightkml/flight.h>

/**
 * A group of flight paths
 */
class FlightGroup {
private:
    /** The flights */
    std::vector<flightkml::Flight> _flights;
public:
    FlightGroup(std::vector<flightkml::Flight>&& flights);

    inline const std::vector<flightkml::Flight>& flights() const {
        return _flights;
    }

    /**
     * Returns the departure time of the flight that departs first
     *
     * If this group has no flight with at least one point, returns a
     * default-constructed ptime
     */
    boost::posix_time::ptime first_departure_time() const;

    /**
     * Returns the arrival time of the flight that arrives last
     *
     * If this group has no flight with at least one point, returns a
     * default-constructed ptime
     */
    boost::posix_time::ptime last_arrival_time() const;
};

#endif
