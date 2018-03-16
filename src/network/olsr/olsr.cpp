#include "olsr.h"
#include "header.h"
#include "multipoint_relay.h"
#include "routing_calc.h"
#include "util/print_container.h"
#include "header/mesh_header.h"
#include <cassert>
#include <limits>
#include <ns3/log.h>
#include <ns3/simulator.h>

NS_LOG_COMPONENT_DEFINE("OLSR");

namespace olsr {

Olsr::Olsr(ns3::Ptr<MeshNetDevice> net_device) :
    _net_device(net_device),
    _hello_interval(ns3::Seconds(10)),
    _topology_control_interval(ns3::Seconds(20)),
    _default_ttl(16),
    // Neighbor table TTL
    _neighbors(ns3::Seconds(120)),
    _mpr_selector(ns3::Seconds(120)),
    _topology(ns3::Seconds(120))
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
    ns3::Simulator::Schedule(_topology_control_interval, &Olsr::SendTopologyControl, this);
}

void Olsr::Send(ns3::Packet packet, IcaoAddress destination) {
    NS_LOG_FUNCTION(this << packet << destination);
    assert(_net_device);
    const auto local_address(_net_device->GetAddress());

    // Check length
    if (packet.GetSize() > static_cast<std::uint32_t>(std::numeric_limits<std::uint16_t>::max())) {
        NS_LOG_WARN("Can't send packet more than 65536 bytes long");
        return;
    }

    // Look up route
    const auto route = _routing.Find(destination);
    if (route != _routing.end()) {
        NS_LOG_LOGIC("Found route via next hop " << route->NextHop() << " to " << destination);

        const auto message(Message::Data(local_address, destination, _default_ttl, static_cast<std::uint16_t>(packet.GetSize())));
        packet.AddHeader(Header(message));
        // Send over first hop
        SendPacket(packet, route->NextHop());
    } else {
        NS_LOG_WARN("At " << _net_device->GetAddress() << ", no route to " << destination);
    }
}

void Olsr::OnPacketReceived(ns3::Packet packet) {
    NS_LOG_FUNCTION(this << packet);

    MeshHeader mesh_header;
    packet.RemoveHeader(mesh_header);

    Header header;
    packet.RemoveHeader(header);
    const auto message_type = header.GetMessage().Type();
    if (message_type == MessageType::Hello) {
        HandleHello(mesh_header.SourceAddress(), header.GetMessage().Neighbors());
    } else if (message_type == MessageType::TopologyControl) {
        HandleTopologyControl(mesh_header.SourceAddress(), std::move(header.GetMessage()));
    } else if (message_type == MessageType::Data) {
        HandleData(std::move(header.GetMessage()));
    } else {
        NS_LOG_WARN("Got a message with an uknown type " << static_cast<unsigned int>(message_type));
    }
}

void Olsr::SendPacket(ns3::Packet packet, IcaoAddress address) {
    NS_LOG_FUNCTION(this << packet << address);
    assert(_net_device);
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

void Olsr::SendTopologyControl() {
    NS_LOG_INFO("Sending topology control");

    auto packet = ns3::Packet();
    // Non-zero TTL for flooding
    auto message = Message(MessageType::TopologyControl, _default_ttl);
    message.MprSelector() = _mpr_selector;
    message.SetOriginator(_net_device->GetAddress());
    packet.AddHeader(Header(message));
    SendPacket(packet, IcaoAddress::Broadcast());

    ns3::Simulator::Schedule(_topology_control_interval, &Olsr::SendTopologyControl, this);
}

void Olsr::HandleTopologyControl(IcaoAddress sender, Message&& message) {
    _topology.RemoveExpired();
    const auto& message_table = message.MprSelector();
    auto in_table = _topology.Find(message.Originator());
    if (in_table != _topology.end()) {
        // Check sequence number
        if (in_table->Sequence() > message_table.Sequence()) {
            // Message is out of order, ignore
            return;
        } else if (in_table->Sequence() == message_table.Sequence()) {
            // This message replaces the old entry
            _topology.Remove(in_table);
        }
    }

    for (auto& entry : message_table) {
        auto& mpr_entry = entry.second;
        auto in_table = _topology.Find(mpr_entry.Address());
        if (in_table != _topology.end()) {
            if (in_table->LastHop() == message.Originator()) {
                in_table->MarkSeen();
            } else {
                // Update last hop
                in_table->SetLastHop(message.Originator());
                in_table->MarkSeen();
            }
        } else {
            // Not in table, insert
            _topology.Insert(TopologyTable::Entry(mpr_entry.Address(), message.Originator(), message_table.Sequence()));
        }
    }

    // Forward message
    if (message.Ttl() > 0) {
        message.DecrementTtl();
        // Resend to each of the multipoint relay neighbors
        for (const auto& entry : _neighbors) {
            const auto& neighbor_entry = entry.second;
            if (neighbor_entry.State() == LinkState::MultiPointRelay) {
                NS_LOG_LOGIC("Forwarding topology control message to " << neighbor_entry.Address());
                ns3::Packet packet;
                packet.AddHeader(Header(message));
                SendPacket(packet, neighbor_entry.Address());
            }
        }
    }

    // Update all the routing
    calculate_routes(&_routing, _neighbors, _topology);
    NS_LOG_INFO(_net_device->GetAddress() << " routing table:");
    NS_LOG_INFO(RoutingTable::PrintTable(_routing));
}

void Olsr::HandleHello(IcaoAddress sender, const NeighborTable& sender_neighbors) {
    UpdateNeighbors(sender, sender_neighbors);
    UpdateMprSelector(sender, sender_neighbors);
    NS_LOG_INFO(DumpState(*this));
}

void Olsr::UpdateNeighbors(IcaoAddress sender, const NeighborTable& sender_neighbors) {
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

    // Update this node's multipoint relay set
    update_multipoint_relay(&_neighbors);
}

void Olsr::UpdateMprSelector(IcaoAddress sender, const NeighborTable& sender_neighbors) {
    // Remove old entries
    _mpr_selector.RemoveExpired();

    const auto local_address = _net_device->GetAddress();
    const auto self_in_sender_neighbors = sender_neighbors.Find(local_address);
    if (self_in_sender_neighbors != sender_neighbors.end()) {
        if (self_in_sender_neighbors->second.State() == LinkState::MultiPointRelay) {
            // This is a multpoint relay of the sender
            const auto in_mpr_selector = _mpr_selector.Find(sender);
            if (in_mpr_selector != _mpr_selector.end()) {
                in_mpr_selector->second.MarkSeen();
            } else {
                _mpr_selector.Insert(sender);
                _mpr_selector.IncrementSequence();
            }
        }
    }
}

Olsr::DumpState::DumpState(const Olsr& olsr) :
    _olsr(olsr)
{
}

std::ostream& operator << (std::ostream& stream, const Olsr::DumpState& state) {
    const auto& olsr = state._olsr;
    stream << "OLSR {\n";
    stream << "Neighbors: {\n";
    for (const auto& entry : olsr.Neighbors()) {
        stream << "    " << entry.second << '\n';
    }
    stream << "}\nMPR selector: sequence " << std::dec << static_cast<unsigned int>(olsr.MprSelector().Sequence()) << " {\n";
    for (const auto& entry : olsr.MprSelector()) {
        stream << "    " << entry.second << '\n';
    }
    stream << "}\nRouting table:\n" << RoutingTable::PrintTable(olsr.Routing()) << "\n}";
    return stream;
}


ns3::TypeId Olsr::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("olsr::Olsr")
        .SetParent<Object>()
        .AddConstructor<Olsr>();
    return id;
}

}
