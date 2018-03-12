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

    // Handle sender
    const auto local_address = _net_device->GetAddress();

    const auto sender_is_neighbor = _neighbors.find(sender) != _neighbors.end();
    auto sender_is_unidirectional = _unidirectional_neighbors.find(sender)
        != _unidirectional_neighbors.end();

    // If sender is not a neighbor or unidirectional neighbor,
    // it becomes a unidirectional neighbor
    if (!sender_is_neighbor && !sender_is_unidirectional) {
        NS_LOG_INFO(local_address << ": adding " << sender << " as unidirectional neighbor");
        _unidirectional_neighbors.insert(sender);
        sender_is_unidirectional = true;
    } else if (neighbors.find(local_address) != neighbors.end()
        || _unidirectional_neighbors.find(local_address)
        != unidirectional_neighbors.end()) {
        // If the sender reported this node as a neighbor or unidirectional
        // neighbor, make the sender a bidirectional neighbor
        NS_LOG_INFO(local_address << ": upgrading " << sender << " to full neighbor");
        const auto in_unidirectional = _unidirectional_neighbors.find(sender);
        if (in_unidirectional != _unidirectional_neighbors.end()) {
            _unidirectional_neighbors.erase(in_unidirectional);
        }
        _neighbors.insert(sender);
    }
}



ns3::TypeId Olsr::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("olsr::Olsr")
        .SetParent<Object>()
        .AddConstructor<Olsr>();
    return id;
}

}
