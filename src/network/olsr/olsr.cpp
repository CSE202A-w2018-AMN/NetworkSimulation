#include "olsr.h"
#include "header.h"
#include "util/print_container.h"
#include "header/mesh_header.h"
#include <ns3/log.h>
#include <ns3/simulator.h>

NS_LOG_COMPONENT_DEFINE("OLSR");

namespace olsr {

Olsr::Olsr(ns3::Ptr<MeshNetDevice> net_device) :
    _net_device(net_device),
    _hello_interval(ns3::Seconds(10))
{
    if (_net_device) {
        _net_device->SetReceiveCallback(std::bind(&Olsr::OnPacketReceived, this, std::placeholders::_1));
    }
}

void Olsr::SetNetDevice(ns3::Ptr<MeshNetDevice> net_device) {
    _net_device = net_device;
    if (_net_device) {
        _net_device->SetReceiveCallback(std::bind(&Olsr::OnPacketReceived, this, std::placeholders::_1));
    }
}

void Olsr::Start() {
    ns3::Simulator::Schedule(_hello_interval, &Olsr::SendHello, this);
}

void Olsr::OnPacketReceived(ns3::Packet packet) {
    NS_LOG_FUNCTION(this << packet);

    MeshHeader mesh_header;
    packet.RemoveHeader(mesh_header);

    Header header;
    packet.RemoveHeader(header);
    if (header.GetMessage().Type() == MessageType::Hello) {
        HandleHello(mesh_header.SourceAddress(), header.GetMessage().Neighbors(), header.GetMessage().UnidirectionalNeighbors());
    }
}

void Olsr::SendPacket(ns3::Packet packet, IcaoAddress address) {
    _net_device->Send(packet, address);
}

void Olsr::SendHello() {
    NS_LOG_INFO("Sending hello");
    auto packet = ns3::Packet();
    auto message = Message(MessageType::Hello);

    // Add neighbors
    message.Neighbors() = _neighbors;
    message.UnidirectionalNeighbors() = _unidirectional_neighbors;

    packet.AddHeader(Header(message));
    SendPacket(packet, IcaoAddress::Broadcast());

    // Schedule next
    ns3::Simulator::Schedule(_hello_interval, &Olsr::SendHello, this);
}

void Olsr::HandleHello(IcaoAddress sender, const std::set<IcaoAddress>& neighbors, const std::set<IcaoAddress>& unidirectional_neighbors) {
    NS_LOG_INFO("Handling hello from " << sender << " with neighbors " << print_container::print(neighbors) << ", unidirectional neighbors " << print_container::print(unidirectional_neighbors));
}



ns3::TypeId Olsr::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("olsr::Olsr")
        .SetParent<Object>()
        .AddConstructor<Olsr>();
    return id;
}

}
