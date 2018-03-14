#include "olsr.h"
#include "header.h"
#include "multipoint_relay.h"
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
        HandleHello(mesh_header.SourceAddress(), header.GetMessage().Neighbors());
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

    packet.AddHeader(Header(message));
    SendPacket(packet, IcaoAddress::Broadcast());

    // Schedule next
    ns3::Simulator::Schedule(_hello_interval, &Olsr::SendHello, this);
}

void Olsr::HandleHello(IcaoAddress sender, const NeighborTable& sender_neighbors) {
    const auto local_address = _net_device->GetAddress();
    NS_LOG_INFO(local_address << " handling hello from " << sender
        << " with neighbors " << print_container::print(sender_neighbors));

    _neighbors.RemoveExpired();

    // Update neighbors of this
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

        // Update 2-hop neighbors
        table_entry.TwoHopNeighbors().clear();
        for (const auto& neighbor_entry : sender_neighbors) {
            const auto address = neighbor_entry.first;
            if (address != local_address) {
                table_entry.TwoHopNeighbors().insert(address);
            }
        }

        table_entry.MarkSeen();
    } else {
        // Nothing here, add an entry
        LinkState new_link_state;
        if (sender_neighbors.Find(local_address) != sender_neighbors.end()) {
            // Other is aware of this, so the link is bidirectional
            new_link_state = LinkState::Bidirectional;
            NS_LOG_INFO(local_address << ": adding bidirectional neighbor "
                << sender);
        } else {
            // Link is unidirectional
            new_link_state = LinkState::Unidirectional;
            NS_LOG_INFO(local_address << ": adding unidirectional neighbor "
                << sender);
        }
        // Build an entry containing the 2-hop neighbors
        auto entry = NeighborTableEntry(sender, new_link_state);

        for (const auto& neighbor_entry : sender_neighbors) {
            const auto address = neighbor_entry.first;
            if (address != local_address) {
                entry.TwoHopNeighbors().insert(address);
            }
        }
        _neighbors.Insert(entry);
    }

    update_multipoint_relay(&_neighbors);
}



ns3::TypeId Olsr::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("olsr::Olsr")
        .SetParent<Object>()
        .AddConstructor<Olsr>();
    return id;
}

}
