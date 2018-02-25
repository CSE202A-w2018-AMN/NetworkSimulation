#include "flightkml.h"

#include <kml/dom.h>

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace flightkml {

Flight::Flight(std::vector<Point>&& points) :
    _points(std::move(points))
{

}

Flight Flight::read_from_kml(const std::string& path) {
    // Read whole file to string
    std::ifstream file;
    file.exceptions(std::ios::badbit | std::ios::failbit);
    file.open(path);

    std::stringstream contents;
    contents << file.rdbuf();

    std::string errors;
    const auto xml_root = kmldom::Parse(contents.str(), &errors);
    if (!xml_root) {
        throw std::runtime_error(errors);
    }
    const auto kml_root = kmldom::AsKml(xml_root);
    if (!kml_root) {
        throw std::runtime_error("Document root is not KML");
    }
    if (!kml_root->has_feature()) {
        throw std::runtime_error("KML has no feature");
    }
    const auto feature = kml_root->get_feature();
    if (feature->has_name()) {
        std::cout << "Feature name: " << feature->get_name() << '\n';
    }

    return Flight(std::vector<Point>());
}

}
