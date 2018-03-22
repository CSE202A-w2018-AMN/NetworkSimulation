#include "dream.h"
#include <cassert>
#include <limits>
#include "network/olsr/routing_calc.h"
#include "header/mesh_header.h"
#include "header.h"
#include <ns3/log.h>
#include <ns3/simulator.h>
#include <cmath>

NS_LOG_COMPONENT_DEFINE("DREAM");

#define ADDR_LOG_INFO(items) NS_LOG_INFO('[' << _net_device->GetAddress() << "] " << items)
#define ADDR_LOG_WARN(items) NS_LOG_WARN('[' << _net_device->GetAddress() << "] " << items)

namespace dream {

namespace {

/**
 * Calculates the length of a vector
 *
 * Note: ns3::Vector3D::GetLength() is not available in ns3 3.26.
 */
double VectorLength(const ns3::Vector& v) {
    // Ideally would use a method that avoids overflow
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

}

Dream::Dream(ns3::Ptr<MeshNetDevice> net_device) :
    _net_device(net_device),
    // Entry TTL values
    _routing(ns3::Hours(1)),
    _neighbors(ns3::Minutes(30)),
    _default_ttl(3),
    // Intervals
    _hello_interval(ns3::Minutes(10)),
    _cleanup_interval(ns3::Minutes(10)),
    _frequent_position_interval(ns3::Minutes(20)),
    _infrequent_position_interval(ns3::Minutes(50)),
    _frequent_max_distance(/* 900 km */900000),
    _infrequent_max_distance(/* 4000 km */400000)
{
    if (_net_device) {
        _net_device->SetReceiveCallback(std::bind(&Dream::OnPacketReceived, this, std::placeholders::_1));
    }
}

void Dream::SetNetDevice(ns3::Ptr<MeshNetDevice> net_device) {
    _net_device = net_device;
    if (_net_device) {
        _net_device->SetReceiveCallback(std::bind(&Dream::OnPacketReceived, this, std::placeholders::_1));
    }
}

void Dream::Start() {
    ns3::Simulator::Schedule(_hello_interval, &Dream::SendHello, this);
    ns3::Simulator::Schedule(_frequent_position_interval, &Dream::SendFrequentPosition, this);
    ns3::Simulator::Schedule(_infrequent_position_interval, &Dream::SendInfrequentPosition, this);
    ns3::Simulator::Schedule(_cleanup_interval, &Dream::Cleanup, this);
}

void Dream::Send(ns3::Packet packet, IcaoAddress destination) {
    // Check length
     if (packet.GetSize() > static_cast<std::uint32_t>(std::numeric_limits<std::uint16_t>::max())) {
         ADDR_LOG_WARN("Can't send packet more than 65536 bytes long");
         return;
     }
     // Add header
     const auto message = Message::DataMessage(_net_device->GetAddress(), destination, _default_ttl, packet.GetSize());
     packet.AddHeader(Header(message));
     RecordPacketSent(packet.GetUid(), PacketRecorder::PacketType::Data);
     SendWithHeader(packet, destination);
}

void Dream::SendWithHeader(ns3::Packet packet, IcaoAddress destination) {
    NS_LOG_FUNCTION(this << packet << destination);
    assert(_net_device);
    const auto local_address(_net_device->GetAddress());

    const auto receiver_info = _routing.Find(destination);      //D get location information of the destination in the table
    if (receiver_info == _routing.end()) {
        ADDR_LOG_WARN("At " << local_address << ", no route to " << destination);
        return;
    }

    //D for calculating alpha
    const auto SenderCoor = _net_device->GetMobilityModel()->GetPosition();
    const auto ReceiverCoor = receiver_info->Location();
    const float r = ns3::CalculateDistance(SenderCoor, ReceiverCoor);       //D Distance(SenderCoor, ReceiverCoor);      //D distance between the device and the destination
    const float x = VectorLength(receiver_info->Velocity()) * (ns3::Simulator::Now().GetSeconds()-receiver_info->LastTime().GetSeconds());        //D maximum distance that the receiver can travel during the time

    if (x < r)
    {
        const float alpha = fabs(asin(x/r));      //D use arcsin

        for (auto iter = _neighbors.begin(); iter != _neighbors.end(); iter++)
        {
            //D calculate the angle between vector (sender, receiver) and (sender, neighbor) using the law of Cosine
            float a = ns3::CalculateDistance(SenderCoor, ReceiverCoor);
            float b = ns3::CalculateDistance(SenderCoor, iter->second.Location());
            float c = ns3::CalculateDistance(ReceiverCoor, iter->second.Location());
            float angle_C = fabs(acos((a*a + b*b - c*c)/(2*a*b)));
            if (alpha >= angle_C)
                SendPacket(packet, iter->second.Address());
        }
    }
    else        //D the destination can be every direction! need to send the packet to all neighbors
    {
        SendPacket(packet, IcaoAddress::Broadcast());
    }
}

void Dream::OnPacketReceived(ns3::Packet packet) {
    NS_LOG_FUNCTION(this << packet);
    MeshHeader mesh_header;
    packet.RemoveHeader(mesh_header);

    Header header;
    packet.RemoveHeader(header);
    const auto message_type = header.GetMessage().GetType();
    ADDR_LOG_INFO(message_type << " message from " << mesh_header.SourceAddress() << " packet " << packet.GetUid());
    if (message_type == Message::Type::Hello) {
        RecordPacketReceived(packet.GetUid());
        HandleHello(mesh_header.SourceAddress(), header.GetMessage().Position());
    } else if (message_type == Message::Type::Position) {
        RecordPacketReceived(packet.GetUid());
        HandlePosition(mesh_header.SourceAddress(), std::move(header.GetMessage()));
    } else if (message_type == Message::Type::Data) {
        HandleData(packet, std::move(header.GetMessage()));
    } else {
        ADDR_LOG_WARN("Got a message with an uknown type " << static_cast<unsigned int>(message_type));
    }
}

void Dream::SendPacket(ns3::Packet packet, IcaoAddress address) {
    NS_LOG_FUNCTION(this << packet << address);
    assert(_net_device);
    _net_device->Send(packet, address);
}

void Dream::HandleHello(IcaoAddress sender, const ns3::Vector& position) {
    NS_LOG_FUNCTION(this << sender << position);
    ADDR_LOG_INFO("Handling hello from " << sender);
    auto table_entry = _neighbors.Find(sender);
    if (table_entry != _neighbors.end()) {
        auto& entry = table_entry->second;
        entry.SetLocation(position);
        entry.MarkSeen();
    } else {
        _neighbors.Insert(NeighborTableEntry(sender, position));
    }
}
void Dream::HandlePosition(IcaoAddress sender, Message&& message) {
    NS_LOG_FUNCTION(this << sender);
    ADDR_LOG_INFO("Handling position from " << message.Origin());
    // Update routing table
    auto table_entry = _routing.Find(message.Origin());
    if (table_entry != _routing.end()) {
        table_entry->SetLocation(message.Position());
        table_entry->SetVelocity(message.Velocity());
        table_entry->MarkSeen();
    } else {
        _routing.Insert(RoutingTable::Entry(message.Origin(), message.Position(), message.Velocity()));
    }
    // Potentially forward
    const auto local_position = _net_device->GetMobilityModel()->GetPosition();
    const auto distance_from_sender = ns3::CalculateDistance(local_position, message.Position());
    if (message.Ttl() > 0 && distance_from_sender < message.MaxDistance()) {
        message.DecrementTtl();
        ns3::Packet packet;
        packet.AddHeader(Header(message));
        // Forward to neighbors
        // Could change to not send back to the server
        SendPacket(packet, IcaoAddress::Broadcast());
    }
}
void Dream::HandleData(ns3::Packet packet, Message&& message) {
    NS_LOG_FUNCTION(this);
    ADDR_LOG_INFO("Handling data that originated at " << message.Origin());
    const auto local_address = _net_device->GetAddress();
    if (message.Destination() == local_address) {
        ADDR_LOG_INFO("Data arrived at " << local_address << " from " << message.Origin());
        RecordPacketReceived(packet.GetUid());
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

void Dream::SendPosition(double max_distance) {
    const auto local_address = _net_device->GetAddress();
    const auto mobility = _net_device->GetMobilityModel();
    const auto message = Message::PositionMessage(
        local_address,
        _default_ttl,
        mobility->GetPosition(),
        mobility->GetVelocity(),
        max_distance
    );
    ns3::Packet packet;
    packet.AddHeader(Header(message));
    RecordPacketSent(packet.GetUid(), PacketRecorder::PacketType::Management);
    SendPacket(packet, IcaoAddress::Broadcast());
}

void Dream::SendHello() {
    ADDR_LOG_INFO("Sending hello");
    const auto mobility = _net_device->GetMobilityModel();
    const auto message = Message::HelloMessage(mobility->GetPosition());
    ns3::Packet packet;
    packet.AddHeader(Header(message));
    // Send to all neighbors
    RecordPacketSent(packet.GetUid(), PacketRecorder::PacketType::Management);
    SendPacket(packet, IcaoAddress::Broadcast());

    ns3::Simulator::Schedule(_hello_interval, &Dream::SendHello, this);
}
void Dream::SendInfrequentPosition() {
    ADDR_LOG_INFO("Sending infrequent position");
    SendPosition(_infrequent_max_distance);
    ns3::Simulator::Schedule(_infrequent_position_interval, &Dream::SendInfrequentPosition, this);
}
void Dream::SendFrequentPosition() {
    ADDR_LOG_INFO("Sending frequent position");
    SendPosition(_frequent_max_distance);
    ns3::Simulator::Schedule(_frequent_position_interval, &Dream::SendFrequentPosition, this);
}

void Dream::Cleanup() {
    NS_LOG_FUNCTION(this);
    _neighbors.RemoveExpired();
    _routing.RemoveExpired();
    ns3::Simulator::Schedule(_cleanup_interval, &Dream::Cleanup, this);
}

void Dream::SetReceiveCallback(receive_callback callback) {
    _receive_callback = callback;
}

ns3::TypeId Dream::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("dream::Dream")
        .SetParent<NetworkProtocol>()
        .AddConstructor<Dream>();
    return id;
}

}
