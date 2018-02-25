
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int main() {
    ns3::Time::SetResolution(ns3::Time::NS);

    ns3::LogComponentEnable("UdpEchoClientApplication", ns3::LOG_LEVEL_INFO);
    ns3::LogComponentEnable("UdpEchoServerApplication", ns3::LOG_LEVEL_INFO);

    ns3::NodeContainer nodes;
    nodes.Create(2);

    ns3::PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", ns3::StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", ns3::StringValue ("2ms"));

    ns3::NetDeviceContainer devices = pointToPoint.Install(nodes);

    ns3::InternetStackHelper stack;
    stack.Install(nodes);

    ns3::Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    ns3::Ipv4InterfaceContainer interfaces = address.Assign(devices);

    ns3::UdpEchoServerHelper echoServer(9);

    ns3::ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(ns3::Seconds(1.0));
    serverApps.Stop(ns3::Seconds(10.0));

    ns3::UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", ns3::UintegerValue(1));
    echoClient.SetAttribute("Interval", ns3::TimeValue(ns3::Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", ns3::UintegerValue(1024));

    ns3::ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(ns3::Seconds(2.0));
    clientApps.Stop(ns3::Seconds(10.0));

    ns3::Simulator::Run();
    ns3::Simulator::Destroy();
    return 0;
}
