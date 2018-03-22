
#include <iostream>
#include <cassert>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "flight_load.h"
#include "flight_mobility.h"
#include "address/icao_address.h"
#include "device/mesh_net_device.h"
#include "application/adsb_sender_helper.h"
#include "ether/ether.h"
#include "recorder/session_recorder.h"
#include "packet_recorder/packet_recorder.h"

#include "network/olsr/olsr.h"
#include "network/dream/dream.h"

#include <ns3/node-container.h>
#include <ns3/mobility-helper.h>
#include <ns3/constant-position-mobility-model.h>
#include <ns3/geographic-positions.h>
#include <ns3/log.h>
#include <ns3/simulator.h>

NS_LOG_COMPONENT_DEFINE("AircraftMeshSimulation");


namespace {

ns3::Ptr<NetworkProtocol> create_protocol() {
    return ns3::CreateObject<olsr::Olsr>();
}

/**
 * Creates and configures aircraft nodes. Returns a container of them.
 */
ns3::NodeContainer create_aircraft(const FlightGroup& flights) {
    const auto first_departure = flights.first_departure_time();
    NS_LOG_INFO("Read " << flights.flights().size() << " flights");
    // Set up a node for each flight
    ns3::NodeContainer nodes;
    nodes.Create(flights.flights().size());

    // Set up mobility helper, which allocates a WaypointMobilityModel
    // for each flight
    ns3::MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::WaypointMobilityModel");
    mobility.Install(nodes);

    // Per-flight setup
    for (std::size_t i = 0; i < flights.flights().size(); i++) {
        auto node = nodes.Get(i);

        // Positions
        const auto& flight = flights.flights()[i];
        auto mobility_model = node->GetObject<ns3::WaypointMobilityModel>();
        assert(!!mobility_model);
        // Fill in waypoints
        fill_flight_waypoints(flight, first_departure, ns3::PeekPointer(mobility_model));

        // Address and network device
        const IcaoAddress address(static_cast<std::uint32_t>(i));
        NS_LOG_INFO("Flight " << i << ": address " << address);
        const ns3::Ptr<MeshNetDevice> device = ns3::CreateObject<MeshNetDevice>();
        device->SetAddress(address);
        device->SetMobilityModel(mobility_model);
        node->AggregateObject(device);

    }

    return nodes;
}

/**
 * Creates and configures ground station nodes
 */
ns3::NodeContainer create_ground_stations() {
    ns3::NodeContainer nodes;

    // Fixed positions
    nodes.Create(1);
    ns3::MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);
    const auto node = nodes.Get(0);

    // Create a node in Iceland
    const auto iceland_latitude = 64.1241;
    const auto iceland_longitude = -21.9187;
    const auto iceland_altitude = 20;
    const auto iceland_ecec = ns3::GeographicPositions::GeographicToCartesianCoordinates(
        iceland_latitude,
        iceland_longitude,
        iceland_altitude,
        ns3::GeographicPositions::WGS84);
    NS_LOG_INFO("Position of Iceland ground station: " << iceland_ecec);
    node->GetObject<ns3::MobilityModel>()->SetPosition(iceland_ecec);
    // Network interface
    const IcaoAddress address(0x800000);
    NS_LOG_INFO("Iceland ground station: address " << address);
    const ns3::Ptr<MeshNetDevice> device = ns3::CreateObject<MeshNetDevice>();
    device->SetAddress(address);
    device->SetMobilityModel(node->GetObject<ns3::MobilityModel>());
    node->AggregateObject(device);

    return nodes;
}

}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: simulation kml-folder-path\n";
        return -1;
    }

    // Simulation configuration
    ns3::Time::SetResolution(ns3::Time::NS);
    ns3::LogComponentEnable("AircraftMeshSimulation", ns3::LOG_ALL);
    ns3::LogComponentEnable("record::SessionRecorder", ns3::LOG_INFO);
    ns3::LogComponentEnable("PacketRecorder", ns3::LOG_LOGIC);
    // ns3::LogComponentEnable("MeshNetDevice", ns3::LOG_LOGIC);
    // ns3::LogComponentEnable("AdsBSender", ns3::LOG_INFO);
    // ns3::LogComponentEnable("Ether", ns3::LOG_LOGIC);
    ns3::LogComponentEnable("OLSR", ns3::LOG_WARN);
    ns3::LogComponentEnable("DREAM", ns3::LOG_WARN);
    // ns3::LogComponentEnable("olsr::NeighborTable", ns3::LOG_LOGIC);
    // ns3::LogComponentEnable("olsr::TopologyTable", ns3::LOG_LOGIC);
    // ns3::LogComponentEnable("olsr::multipoint_relay", ns3::LOG_ALL);

    // Create aircraft and ground stations
    const auto flights = load_flights(argv[1]);
    auto aircraft = create_aircraft(flights);
    auto ground_stations = create_ground_stations();

    // Create ether and container of all nodes
    ns3::NodeContainer all_nodes(aircraft, ground_stations);
    Ether ether;
    // 300 km
    ether.SetRange(300000);
    for (auto iter = all_nodes.Begin(); iter != all_nodes.End(); ++iter) {
        ether.AddDevice((*iter)->GetObject<MeshNetDevice>());
    }

    // Set up network protocol
    for (auto iter = all_nodes.Begin(); iter != all_nodes.End(); ++iter) {
        auto protocol = create_protocol();
        (*iter)->AggregateObject(protocol);
    }

    // Create applications
    AdsBSenderHelper sender_helper(ns3::Minutes(30));
    auto adsb_senders = sender_helper.Install(aircraft);

    // Create recorder
    record::SessionRecorder recorder(flights.first_departure_time(), ns3::Minutes(10), std::move(all_nodes));
    recorder.Start();

    // Create packet recorder
    auto packet_recorder = ns3::CreateObject<PacketRecorder>();
    for (auto iter = all_nodes.Begin(); iter != all_nodes.End(); ++iter) {
        auto node = *iter;
        assert(node);
        auto olsr = node->GetObject<olsr::Olsr>();
        assert(olsr);
        olsr->SetPacketRecorder(packet_recorder);
    }

    adsb_senders.Start(ns3::Seconds(0));

    NS_LOG_INFO("Running simulation");
    ns3::Simulator::Stop(ns3::Hours(36));
    ns3::Simulator::Run();
    ns3::Simulator::Destroy();
    NS_LOG_INFO("Destroyed simulation");

    recorder.WriteJson("log.json");
    packet_recorder->WriteCsv("packets.csv");

    return 0;
}
