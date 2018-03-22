#include "session_recorder.h"
#include "network/olsr/olsr.h"
#include <ns3/simulator.h>
#include <ns3/mobility-model.h>
#include <ns3/node.h>
#include <ns3/log.h>
#include <ns3/geographic-positions.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <boost/math/constants/constants.hpp>

NS_LOG_COMPONENT_DEFINE("record::SessionRecorder");

namespace record {

namespace {

static constexpr double EARTH_RADIUS = 6378137;
static constexpr double WGS84_ECCENTRICITY = 8.1819190842622e-2;

inline double to_degrees(double radians) {
    return radians * 360.0 / boost::math::double_constants::two_pi;
}

/**
 * Converts ECEF coordinates to (latitude, longitude, altitude)
 */
std::tuple<double, double, double> EcefToLla(const ns3::Vector& ecef) {
    constexpr auto asq = EARTH_RADIUS * EARTH_RADIUS;
    constexpr auto esq = WGS84_ECCENTRICITY * WGS84_ECCENTRICITY;
    constexpr auto b = std::sqrt(asq * (1 - esq));
    constexpr auto bsq = b * b;
    constexpr auto ep = std::sqrt((asq - bsq) / bsq);

    const auto x = ecef.x;
    const auto y = ecef.y;
    const auto z = ecef.z;

    const auto p = std::sqrt(x * x + y * y);
    const auto th = std::atan2(EARTH_RADIUS * z, b * p);
    auto longitude = std::atan2(y, x);
    const auto latitude = std::atan2(z + ep * ep * b * std::pow(std::sin(th), 3), p - esq * EARTH_RADIUS * std::pow(std::cos(th), 3));
    const auto n = EARTH_RADIUS / std::sqrt(1 - esq * std::pow(std::sin(latitude), 2));
    const auto altitude = p / std::cos(latitude) - n;

    const auto two_pi = boost::math::double_constants::two_pi;
    longitude = std::fmod(longitude, two_pi);

    // Convert from radians to degrees
    return std::make_tuple(to_degrees(latitude), to_degrees(longitude), altitude);
}

}

SessionRecorder::SessionRecorder(ptime simulation_start, ns3::Time interval, ns3::NodeContainer&& nodes) :
    _simulation_start(simulation_start),
    _interval(interval),
    _nodes(nodes),
    _session()
{
}

void SessionRecorder::Start() {
    // Run at the beginning of the simulation
    ns3::Simulator::Schedule(ns3::Time(), &SessionRecorder::RecordRecord, this);
}

void SessionRecorder::RecordRecord() {
    NS_LOG_INFO(ns3::Simulator::Now().GetHours() << " hours, recording status");
    const auto time = NowRealTime();
    auto record = Record(time);
    for (auto iter = _nodes.Begin(); iter != _nodes.End(); ++iter) {
        const auto mobility = (*iter)->GetObject<ns3::MobilityModel>();
        assert(mobility);
        const auto net_device = (*iter)->GetObject<MeshNetDevice>();
        assert(net_device);
        const auto olsr = (*iter)->GetObject<olsr::Olsr>();
        const auto pos_ecef = mobility->GetPosition();
        double latitude;
        double longitude;
        double altitude;
        std::tie(latitude, longitude, altitude) = EcefToLla(pos_ecef);
        olsr::RoutingTable routing;
        if (olsr) {
            routing = olsr->Routing();
        }
        auto node_record = NodeRecord {
            latitude,
            longitude,
            altitude,
            routing
        };
        record.AddNode(net_device->GetAddress(), std::move(node_record));
    }
    _session.AddRecord(std::move(record));

    // Schedule next
    ns3::Simulator::Schedule(_interval, &SessionRecorder::RecordRecord, this);
}

ptime SessionRecorder::NowRealTime() {
    const auto since_start = ns3::Simulator::Now();
    const auto microseconds_since_start = boost::posix_time::microseconds(
        since_start.ToInteger(ns3::Time::US));
    return _simulation_start + microseconds_since_start;
}

void SessionRecorder::WriteJson(const std::string& path) {
    std::ofstream file;
    file.exceptions(std::ios::badbit | std::ios::failbit);
    file.open(path, std::ios::out | std::ios::trunc);
    _session.PrintJson(file);
}


}
