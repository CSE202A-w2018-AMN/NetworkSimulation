#include "olsr.h"
#include "header.h"
#include "multipoint_relay.h"
#include "routing_calc.h"
#include "util/print_container.h"
#include "header/mesh_header.h"
#include "packet_recorder/packet_recorder.h"
#include <cassert>
#include <limits>
#include <ns3/log.h>
#include <ns3/simulator.h>

NS_LOG_COMPONENT_DEFINE("OLSR");

#define ADDR_LOG_INFO(items) NS_LOG_INFO('[' << _net_device->GetAddress() << "] " << items)
#define ADDR_LOG_WARN(items) NS_LOG_WARN('[' << _net_device->GetAddress() << "] " << items)

namespace olsr {

Olsr::Olsr(ns3::Ptr<MeshNetDevice> net_device) :
    _net_device(net_device),
    _hello_interval(ns3::Minutes(10)),
    _topology_control_interval(ns3::Minutes(10)),
    _cleanup_interval(ns3::Minutes(10)),
    _default_ttl(8),
    // Neighbor table TTL
    _neighbors(ns3::Minutes(21)),
    _mpr_selector(ns3::Minutes(21)),
    _topology(ns3::Minutes(21))
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
    ns3::Simulator::Schedule(_cleanup_interval, &Olsr::Cleanup, this);
}

void Olsr::Send(ns3::Packet packet, IcaoAddress destination) {
    // ADDR_LOG_INFO("Olsr::Send packet " << packet << " to " << destination);
    assert(_net_device);
    // Check length
    if (packet.GetSize() > static_cast<std::uint32_t>(std::numeric_limits<std::uint16_t>::max())) {
        ADDR_LOG_WARN("Can't send packet more than 65536 bytes long");
        return;
    }
    const auto local_address(_net_device->GetAddress());
    const auto message(Message::Data(local_address, destination, _default_ttl, static_cast<std::uint16_t>(packet.GetSize())));
    packet.AddHeader(Header(message));
    RecordPacketSent(packet.GetUid(), PacketRecorder::PacketType::Data);
    SendWithHeader(packet, destination);
}

void Olsr::SendWithHeader(ns3::Packet packet, IcaoAddress destination) {
    // ADDR_LOG_INFO("Olrsr::SendWithHeader " << packet << " to " << destination);
    // Special case for broadcast: Forward to multipoint relay neighbors
    if (destination == IcaoAddress::Broadcast()) {
        SendMultipointRelay(packet);
    } else {
        // Look up route
        const auto route = _routing.Find(destination);
        if (route != _routing.end()) {
            ADDR_LOG_INFO("Found route via next hop " << route->NextHop() << " to " << destination);
            // Send over next hop
            SendPacket(packet, route->NextHop());
        } else {
            ADDR_LOG_INFO("At " << _net_device->GetAddress() << ", no route to " << destination);
        }
    }
}

void Olsr::SetReceiveCallback(receive_callback callback) {
    _receive_callback = callback;
}

void Olsr::OnPacketReceived(ns3::Packet packet) {
    NS_LOG_FUNCTION(this << packet);
    RecordPacketReceived(packet.GetUid());

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
        HandleData(packet, std::move(header.GetMessage()));
    } else {
        ADDR_LOG_WARN("Got a message with an uknown type " << static_cast<unsigned int>(message_type));
    }
}

void Olsr::SendPacket(ns3::Packet packet, IcaoAddress destination) {
    ADDR_LOG_INFO("Olsr::SendPacket " << packet << " to " << destination);
    assert(_net_device);
    _net_device->Send(packet, destination);
}

void Olsr::HandleData(ns3::Packet packet, Message&& message) {
    ADDR_LOG_INFO("HandleData origin " << message.Origin() << " destination " << message.Destination());
    const auto local_address = _net_device->GetAddress();
    if (message.Destination() == local_address) {
        ADDR_LOG_INFO("Data arrived at " << local_address << " from " << message.Origin());
        if (_receive_callback) {
            _receive_callback(packet);
        } else {
            ADDR_LOG_WARN("No receive callback set");
        }
    } else if (message.Ttl() > 0) {
        ADDR_LOG_INFO("Forwarding data");
        message.DecrementTtl();
        packet.AddHeader(Header(message));
        SendWithHeader(packet, message.Destination());
    } else {
        ADDR_LOG_WARN("Data with destination " << message.Destination() << " died at " << local_address);
    }
}

void Olsr::SendHello() {
    // ADDR_LOG_INFO("Sending hello");
    auto packet = ns3::Packet();
    auto message = Message(MessageType::Hello);

    // Add neighbors
    message.Neighbors() = _neighbors;

    packet.AddHeader(Header(message));
    RecordPacketSent(packet.GetUid(), PacketRecorder::PacketType::Management);
    SendPacket(packet, IcaoAddress::Broadcast());

    // Schedule next
    ns3::Simulator::Schedule(_hello_interval, &Olsr::SendHello, this);
}

void Olsr::SendTopologyControl() {
    if (!_mpr_selector.empty()) {
        ADDR_LOG_INFO("Sending topology control");

        auto packet = ns3::Packet();
        // Non-zero TTL for flooding
        auto message = Message(MessageType::TopologyControl, _default_ttl);
        message.MprSelector() = _mpr_selector;
        message.SetOriginator(_net_device->GetAddress());
        packet.AddHeader(Header(message));
        RecordPacketSent(packet.GetUid(), PacketRecorder::PacketType::Management);
        SendPacket(packet, IcaoAddress::Broadcast());

    }
    ns3::Simulator::Schedule(_topology_control_interval, &Olsr::SendTopologyControl, this);
}

void Olsr::HandleTopologyControl(IcaoAddress sender, Message&& message) {
    ADDR_LOG_INFO("HandleTopologyControl originating from " << message.Originator());

    const auto& message_table = message.MprSelector();
    auto in_table = _topology.Find(message.Originator());
    if (in_table != _topology.end()) {
        ADDR_LOG_INFO("Originator is in topology table");
        // Check sequence number
        if (in_table->Sequence() > message_table.Sequence()) {
            // Message is out of order, ignore
            ADDR_LOG_INFO("Out-of-order topology control message");
            return;
        } else if (in_table->Sequence() == message_table.Sequence()) {
            // This message replaces the old entry
            ADDR_LOG_INFO("Removing old topology entry");
            _topology.Remove(in_table);
        }
    }

    for (auto& entry : message_table) {
        auto& mpr_entry = entry.second;
        auto in_table = _topology.Find(mpr_entry.Address());
        if (in_table != _topology.end()) {
            if (in_table->LastHop() == message.Originator()) {
                ADDR_LOG_INFO("Marking entry seen");
                in_table->MarkSeen();
            } else {
                // Update last hop
                ADDR_LOG_INFO("Updating last hop to " << in_table->Destination() << ": old " << in_table->LastHop() << ", new " << message.Originator());
                in_table->SetLastHop(message.Originator());
                in_table->MarkSeen();
            }
        } else {
            // Not in table, insert
            ADDR_LOG_INFO("Inserting into topology table: destination " << mpr_entry.Address() << ", next hop " << message.Originator() << ", sequence " << message_table.Sequence());
            _topology.Insert(TopologyTable::Entry(mpr_entry.Address(), message.Originator(), message_table.Sequence()));
        }
    }

    ADDR_LOG_INFO("Updated topology table:\n" << TopologyTable::PrintTable(_topology));

    // Forward message
    if (message.Ttl() > 0) {
        message.DecrementTtl();
        // Resend to each of the multipoint relay neighbors
        ns3::Packet packet;
        packet.AddHeader(Header(message));
        RecordPacketSent(packet.GetUid(), PacketRecorder::PacketType::Management);
        SendMultipointRelay(packet);
    }
}

void Olsr::SendMultipointRelay(ns3::Packet packet) {
    auto sent = false;
    for (const auto& entry : _neighbors) {
        const auto& neighbor_entry = entry.second;
        if (neighbor_entry.State() == LinkState::MultiPointRelay) {
            sent = true;
            SendPacket(packet, neighbor_entry.Address());
        }
    }
    if (!sent) {
        ADDR_LOG_INFO("No multipoint relay neighbors to send to");
    }
}

void Olsr::HandleHello(IcaoAddress sender, const NeighborTable& sender_neighbors) {
    UpdateNeighbors(sender, sender_neighbors);
    UpdateMprSelector(sender, sender_neighbors);
    ADDR_LOG_INFO(DumpState(*this));
}

void Olsr::UpdateNeighbors(IcaoAddress sender, const NeighborTable& sender_neighbors) {
    const auto local_address = _net_device->GetAddress();
    ADDR_LOG_INFO(local_address << " handling hello from " << sender
        << " with neighbors " << print_container::print(sender_neighbors));

    // Update neighbors of this
    const auto sender_entry = _neighbors.Find(sender);
    if (sender_entry != _neighbors.end()) {
        // Have an entry
        auto& table_entry = sender_entry->second;
        // Make bidirectional if not
        if (table_entry.State() == LinkState::Unidirectional) {
            ADDR_LOG_INFO(local_address << ": upgrading neighbor "
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
            ADDR_LOG_INFO(local_address << ": adding bidirectional neighbor "
                << sender);
        } else {
            // Link is unidirectional
            new_link_state = LinkState::Unidirectional;
            ADDR_LOG_INFO(local_address << ": adding unidirectional neighbor "
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

void Olsr::Cleanup() {
    NS_LOG_FUNCTION(this);
    _neighbors.RemoveExpired();
    _mpr_selector.RemoveExpired();
    _topology.RemoveExpired();
    // Update all the routing
    calculate_routes(&_routing, _neighbors, _topology);
    ADDR_LOG_INFO("Routing table:\n" << RoutingTable::PrintTable(_routing));
    ns3::Simulator::Schedule(_cleanup_interval, &Olsr::Cleanup, this);
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
    stream << "}\n";
    stream << "Topology table:\n" << TopologyTable::PrintTable(olsr.Topology()) << '\n';
    stream << "Routing table:\n" << RoutingTable::PrintTable(olsr.Routing()) << "\n}";
    return stream;
}

IcaoAddress Olsr::Address() const {
    assert(_net_device);
    return _net_device->GetAddress();
}

ns3::TypeId Olsr::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("olsr::Olsr")
        .SetParent<NetworkProtocol>()
        .AddConstructor<Olsr>();
    return id;
}

}
