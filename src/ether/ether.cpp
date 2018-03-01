#include "ether.h"
#include <ns3/log.h>

NS_LOG_COMPONENT_DEFINE("Ether");

void Ether::AddDevice(ns3::Ptr<MeshNetDevice> device) {
    device->SetSendCallback(std::bind(&Ether::OnSend, this, std::placeholders::_1, std::placeholders::_2));
    _devices.push_back(device);
}

void Ether::OnSend(const ns3::Vector& position, ns3::Packet packet) {
    NS_LOG_INFO("Ether send " << position << packet);
}
