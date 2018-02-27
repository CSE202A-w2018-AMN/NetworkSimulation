
#include <iostream>
#include <cassert>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "flight_load.h"
#include "flight_mobility.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"

#include <ns3/mobility-helper.h>

NS_LOG_COMPONENT_DEFINE ("AircraftMeshSimulation");

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: simulation kml-folder-path\n";
        return -1;
    }

    // Simulation configuration
    ns3::Time::SetResolution(ns3::Time::NS);
    ns3::LogComponentEnable("MobilityHelper", ns3::LOG_ALL);
    ns3::LogComponentEnable("UdpEchoClientApplication", ns3::LOG_LEVEL_INFO);
    ns3::LogComponentEnable("UdpEchoServerApplication", ns3::LOG_LEVEL_INFO);

    // Read flights
    const auto flights = load_flights(argv[1]);
    const auto first_departure = flights.first_departure_time();
    std::cout << "Read " << flights.flights().size() << " flights\n";
    std::cout << "Earliest departure time: " << first_departure << '\n';
    std::cout << "Latest arrival time: " << flights.last_arrival_time() << '\n';
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
        const auto& flight = flights.flights()[i];
        auto mobility_model = node->GetObject<ns3::WaypointMobilityModel>();
        assert(!!mobility_model);
        // Fill in waypoints
        fill_flight_waypoints(flight, first_departure, ns3::PeekPointer(mobility_model));
    }

    // Connect all aircraft over a bus with CSMA (temporarily)
    ns3::CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", ns3::StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", ns3::TimeValue(ns3::MilliSeconds(100)));
    ns3::NetDeviceContainer csmaDevices = csma.Install(nodes);

    // Addressing
    ns3::InternetStackHelper stack;
    stack.Install(nodes);
    ns3::Ipv4AddressHelper address_helper;
    address_helper.SetBase("10.0.0.0", "255.0.0.0");
    ns3::Ipv4InterfaceContainer interfaces = address_helper.Assign(csmaDevices);

    // Set up an echo server on flight 0, and an echo client on 1
    if (nodes.GetN() != 0) {
        ns3::UdpEchoServerHelper echoServer(9000);
        ns3::ApplicationContainer serverApps = echoServer.Install(nodes.Get(0));
        serverApps.Start(ns3::Seconds(0));
        serverApps.Stop(ns3::Hours(1));
    }

    if (nodes.GetN() >= 2) {
        ns3::UdpEchoClientHelper client(interfaces.GetAddress(0), 9000);
        client.SetAttribute("MaxPackets", ns3::UintegerValue(10));
        client.SetAttribute("Interval", ns3::TimeValue(ns3::Minutes(1)));
        client.SetAttribute("PacketSize", ns3::UintegerValue(512));

        ns3::ApplicationContainer clientApps = client.Install(nodes.Get(1));
        clientApps.Start(ns3::Seconds(0));
        clientApps.Stop(ns3::Hours(1));
    }

    // ns3::Time::SetResolution(ns3::Time::NS);
    //
    // ns3::LogComponentEnable("UdpEchoClientApplication", ns3::LOG_LEVEL_INFO);
    // ns3::LogComponentEnable("UdpEchoServerApplication", ns3::LOG_LEVEL_INFO);
    //
    // ns3::NodeContainer nodes;
    // nodes.Create(2);
    //
    // ns3::PointToPointHelper pointToPoint;
    // pointToPoint.SetDeviceAttribute("DataRate", ns3::StringValue ("5Mbps"));
    // pointToPoint.SetChannelAttribute("Delay", ns3::StringValue ("2ms"));
    //
    // ns3::NetDeviceContainer devices = pointToPoint.Install(nodes);
    //
    // ns3::InternetStackHelper stack;
    // stack.Install(nodes);
    //
    // ns3::Ipv4AddressHelper address;
    // address.SetBase("10.1.1.0", "255.255.255.0");
    //
    // ns3::Ipv4InterfaceContainer interfaces = address.Assign(devices);
    //
    // ns3::UdpEchoServerHelper echoServer(9);
    //
    // ns3::ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    // serverApps.Start(ns3::Seconds(1.0));
    // serverApps.Stop(ns3::Seconds(10.0));
    //
    // ns3::UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
    // echoClient.SetAttribute("MaxPackets", ns3::UintegerValue(1));
    // echoClient.SetAttribute("Interval", ns3::TimeValue(ns3::Seconds(1.0)));
    // echoClient.SetAttribute("PacketSize", ns3::UintegerValue(1024));
    //
    // ns3::ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    // clientApps.Start(ns3::Seconds(2.0));
    // clientApps.Stop(ns3::Seconds(10.0));

    ns3::Simulator::Run();
    ns3::Simulator::Destroy();
    return 0;
}
