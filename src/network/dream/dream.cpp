#include "dream.h"
#include <cassert>
#include <limits>
#include "network/olsr/routing_calc.h"
#include <ns3/log.h>
#include <ns3/simulator.h>
#include <math.h>

NS_LOG_COMPONENT_DEFINE("DREAM");

namespace dream {

Dream::Dream(ns3::Ptr<MeshNetDevice> net_device) :
    _net_device(net_device)
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
    // ns3::Simulator::Schedule(_hello_interval, &Dream::SendHello, this);
    // ns3::Simulator::Schedule(_topology_control_interval, &Dream::SendTopologyControl, this);
}

/*D
float Dream::Distance(const auto point1, const auto point2)
{
    return sqrt((point1.X-point2.X)*(point1.X-point2.X) + (point1.Y-point2.Y)*(point1.X-point2.Y));
}
D*/

void Dream::Send(ns3::Packet packet, IcaoAddress destination) {
    NS_LOG_FUNCTION(this << packet << destination);
    assert(_net_device);
    const auto local_address(_net_device->GetAddress());
    //
    // // Check length
     if (packet.GetSize() > static_cast<std::uint32_t>(std::numeric_limits<std::uint16_t>::max())) {
         NS_LOG_WARN("Can't send packet more than 65536 bytes long");
         return;
     }


    const auto receiver_info = _routing.Find(destination);      //D get location information of the destination in the table
    if (receiver_info == _routing.end()) {
        NS_LOG_WARN("At " << _net_device->GetAddress() << ", no route to " << destination);
        return;
    }

    //D for calculating alpha
    const auto SenderCoor = _net_device->GetMobilityModel()->GetPosition();->GetCoordination();
    const auto ReceiverCoor = receiver_info->GetCoordination();
    const float r = Distance(SenderCoor, ReceiverCoor);      //D distance between the device and the destination
    const float x = receiver_info->velocity() * (ns3::Simulator::Now() - receiver_info->last_time());        //D maximum distance that the receiver can travel during the time

    if (x < r)
    {
        const float alpha = fabs(asin(x/r));      //D use arcsin

        for (auto iter = _neighbors.begin(); iter != _neighbors.end(); iter++)
        {
            //D calculate the angle between vector (sender, receiver) and (sender, neighbor) using the law of Cosine
            float a = Distance(SenderCoor, ReceiverCoor);
            float b = Distance(SenderCoor, iter->GetCoordination);
            float c = Distance(ReceiverCoor, iter->GetCoordination);
            float angle_C = fabs(acos((a*a + b*b - c*c)/(2*a*b)));
            if (alpha >= angle_C)
                SendPacket(packet, iter->GetAddress);
        }
    }
    else        //D the destination can be every direction! need to send the packet to all neighbors
    {
        for (auto iter = _neighbors.begin(); iter != _neighbors.end(); iter++)
        {
            SendPacket(packet, iter->GetAddress);
        }
    }

}

void Dream::OnPacketReceived(ns3::Packet packet) {
    NS_LOG_FUNCTION(this << packet);

    // MeshHeader mesh_header;
    // packet.RemoveHeader(mesh_header);
    //
    // Header header;
    // packet.RemoveHeader(header);
    // const auto message_type = header.GetMessage().Type();
    // if (message_type == MessageType::Hello) {
    //     HandleHello(mesh_header.SourceAddress(), header.GetMessage().Neighbors());
    // } else if (message_type == MessageType::TopologyControl) {
    //     HandleTopologyControl(mesh_header.SourceAddress(), std::move(header.GetMessage()));
    // } else if (message_type == MessageType::Data) {
    //     HandleData(std::move(header.GetMessage()));
    // } else {
    //     NS_LOG_WARN("Got a message with an uknown type " << static_cast<unsigned int>(message_type));
    // }
}

void Dream::SendPacket(ns3::Packet packet, IcaoAddress address) {
    NS_LOG_FUNCTION(this << packet << address);
    assert(_net_device);
    _net_device->Send(packet, address);
}


ns3::TypeId Dream::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("dream::Dream")
        .SetParent<Object>()
        .AddConstructor<Dream>();
    return id;
}

}
