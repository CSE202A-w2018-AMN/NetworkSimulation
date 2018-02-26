#include "flight_group.h"

FlightGroup::FlightGroup(std::vector<flightkml::Flight>&& flights) :
    _flights(flights)
{
}

boost::posix_time::ptime FlightGroup::first_departure_time() const {
    auto first = boost::posix_time::ptime(boost::posix_time::pos_infin);
    for (const auto& flight : _flights) {
        const auto departure = flight.departure_time();
        if (!departure.is_special() && departure < first) {
            first = departure;
        }
    }
    if (first.is_special()) {
        return boost::posix_time::ptime();
    } else {
        return first;
    }
}

boost::posix_time::ptime FlightGroup::last_arrival_time() const {
    auto last = boost::posix_time::ptime(boost::posix_time::neg_infin);
    for (const auto& flight : _flights) {
        const auto arrival = flight.arrival_time();
        if (!arrival.is_special() && arrival > last) {
            last = arrival;
        }
    }
    if (last.is_special()) {
        return boost::posix_time::ptime();
    } else {
        return last;
    }
}
