
#include <iostream>

#include "flight.h"


int main() {
    const auto flight = flightkml::Flight::read_from_kml("/home/samcrow/Documents/CurrentClasses/CSE 222A/Project/simulation/flights/FlightAware_AFR84_LFPG_KSFO_20180224.kml");
    std::cout << "Read " << flight.points().size() << " points\n";
}
