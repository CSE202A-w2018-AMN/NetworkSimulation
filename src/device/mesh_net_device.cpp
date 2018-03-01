#include "mesh_net_device.h"
#include "header/mesh_header.h"
#include <ns3/log.h>

NS_LOG_COMPONENT_DEFINE("MeshNetDevice");

MeshNetDevice::MeshNetDevice() {}

IcaoAddress MeshNetDevice::GetAddress() const {
    return _address;
}

void MeshNetDevice::SetAddress(IcaoAddress address) {
    _address = address;
}

void MeshNetDevice::Send(ns3::Packet packet, IcaoAddress destination) {
    NS_LOG_FUNCTION(this << packet << destination);
    if (!_mobility) {
        NS_LOG_ERROR("Mobility model missing, cannot send packet");
        return;
    }
    NS_LOG_INFO("Sending " << packet.GetSize() << " bytes " << _address << " => " << destination);

    const auto position = _mobility->GetPosition();
    NS_LOG_INFO("Current position: " << position);

    MeshHeader header(_address, destination);
    packet.AddHeader(header);
    if (_send_callback) {
        _send_callback(position, packet);
    } else {
        NS_LOG_INFO("No send callback set, not sending packet");
    }
}

void MeshNetDevice::SetSendCallback(send_callback callback) {
    _send_callback = callback;
}

void MeshNetDevice::SetMobilityModel(ns3::Ptr<ns3::MobilityModel> mobility) {
    _mobility = mobility;
}

void MeshNetDevice::Receive(ns3::Packet packet) {
    NS_LOG_FUNCTION(this << packet);
}

ns3::TypeId MeshNetDevice::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("MeshNetDevice")
        .SetParent<Object>()
        .AddConstructor<MeshNetDevice>();
    return id;
}
