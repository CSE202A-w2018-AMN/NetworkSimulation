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
    _hello_interval(ns3::Seconds(10)),
    // Neighbor table TTL
    _neighbors(ns3::Seconds(120))
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
    message.Neighbors() = _neighbors.Neighbors();
    message.UnidirectionalNeighbors() = _neighbors.UnidirectionalNeighbors();

    packet.AddHeader(Header(message));
    SendPacket(packet, IcaoAddress::Broadcast());

    // Schedule next
    ns3::Simulator::Schedule(_hello_interval, &Olsr::SendHello, this);
}

void Olsr::HandleHello(IcaoAddress sender, const std::set<IcaoAddress>& neighbors, const std::set<IcaoAddress>& unidirectional_neighbors) {
    const auto local_address = _net_device->GetAddress();
    NS_LOG_INFO(local_address << " handling hello from " << sender
        << " with neighbors " << print_container::print(neighbors)
        << ", unidirectional neighbors "
        << print_container::print(unidirectional_neighbors));

    _neighbors.RemoveExpired();

    const auto sender_entry = _neighbors.Find(sender);
    if (sender_entry != _neighbors.end()) {
        // Have an entry
        auto& table_entry = sender_entry->second;
        // Make bidirectional if not
        if (table_entry.State() == LinkState::Unidirectional) {
            NS_LOG_INFO(local_address << ": upgrading neighbor "
                << sender << " to bidirectional");
            table_entry.SetState(LinkState::Bidirectional);
        }
        table_entry.MarkSeen();
    } else {
        // Nothing here
        if (neighbors.find(local_address) != neighbors.end()
            || unidirectional_neighbors.find(local_address)
            != unidirectional_neighbors.end()) {
            // Other is aware of this, so the link is bidirectional
            _neighbors.Insert(NeighborTableEntry(sender, LinkState::Bidirectional));
            NS_LOG_INFO(local_address << ": adding bidirectional neighbor "
                << sender);
        } else {
            // Link is unidirectional
            _neighbors.Insert(NeighborTableEntry(sender, LinkState::Unidirectional));
            NS_LOG_INFO(local_address << ": adding unidirectional neighbor "
                << sender);
        }
    }
}



ns3::TypeId Olsr::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("olsr::Olsr")
        .SetParent<Object>()
        .AddConstructor<Olsr>();
    return id;
}

}
