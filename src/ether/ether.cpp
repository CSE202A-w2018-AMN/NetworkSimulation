#include "ether.h"
#include <ns3/log.h>
#include <limits>

NS_LOG_COMPONENT_DEFINE("Ether");

Ether::Ether() :
    _range(std::numeric_limits<double>::infinity())
{
    NS_LOG_FUNCTION(this);
}

void Ether::AddDevice(ns3::Ptr<MeshNetDevice> device) {
    NS_LOG_FUNCTION(this << device);
    device->SetSendCallback(std::bind(&Ether::OnSend, this, std::placeholders::_1, std::placeholders::_2));
    _devices.push_back(device);
}

void Ether::OnSend(const ns3::Vector& position, ns3::Packet packet) {
    NS_LOG_FUNCTION(this << position << packet);
    NS_LOG_INFO("Ether send " << position << packet);
    // Find all devices in range
    for (auto& other_device : _devices) {
        const auto other_position = other_device->GetMobilityModel()->GetPosition();
        const auto distance = ns3::CalculateDistance(position, other_position);
        if (distance <= _range) {
            other_device->Receive(packet);
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
