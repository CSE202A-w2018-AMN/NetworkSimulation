#include "flight_load.h"
#include <boost/filesystem.hpp>
using flightkml::Flight;

FlightGroup load_flights(const std::string& directory) {
    auto flights = std::vector<Flight>();

    for (const auto& entry : boost::filesystem::directory_iterator(directory)) {
        if (boost::filesystem::is_regular_file(entry)) {
            const auto flight = Flight::read_from_kml(entry.path().native());
            flights.push_back(std::move(flight));
        }
    }
    return FlightGroup(std::move(flights));
}
