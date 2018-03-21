#include "ether.h"
#include <ns3/log.h>
#include <ns3/simulator.h>
#include <limits>

NS_LOG_COMPONENT_DEFINE("Ether");

namespace {

/** Speed of light, meters/second */
static const double SPEED_OF_LIGHT = 299792000.0;

/** Bit rate, bits/second */
static const double BITS_PER_SECOND = 100000;

}

Ether::Ether() :
    _range(std::numeric_limits<double>::infinity())
{
    NS_LOG_FUNCTION(this);
}

void Ether::AddDevice(ns3::Ptr<MeshNetDevice> device) {
    NS_LOG_FUNCTION(this << device);
    device->SetSendCallback(std::bind(&Ether::OnSend, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _devices.push_back(device);
}

void Ether::OnSend(const MeshNetDevice* sender, const ns3::Vector& position, ns3::Packet packet) {
    NS_LOG_FUNCTION(this << position << packet);
    // Find all devices in range
    for (auto& other_device : _devices) {
        // Sender does not receive
        if (other_device != sender) {
            const auto other_position = other_device->GetMobilityModel()->GetPosition();
            const auto distance = ns3::CalculateDistance(position, other_position);
            NS_LOG_LOGIC("Distance between nodes " << sender->GetAddress() << " and " << other_device->GetAddress() << ": " << distance << " m");
            if (distance <= _range) {
                // Calculate propagation time and transmission time
                const double propagation_seconds = distance / SPEED_OF_LIGHT;
                const double sending_seconds = static_cast<double>(packet.GetSize()) / BITS_PER_SECOND;
                const auto receive_delay = ns3::Time::FromDouble(propagation_seconds + sending_seconds, ns3::Time::Unit::S);
                NS_LOG_LOGIC("Receive delay " << sender->GetAddress() << " -> " << other_device->GetAddress() << ": " << receive_delay);
                NS_LOG_LOGIC("Before sending, time is " << ns3::Simulator::Now());
                ns3::Simulator::Schedule(receive_delay, &MeshNetDevice::Receive, other_device, packet);
            }
        }
    }
}

void Ether::SetRange(double range) {
    NS_LOG_FUNCTION(this << range);
    _range = range;
}

double Ether::GetRange() const {
    return _range;
}
