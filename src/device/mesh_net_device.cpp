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
    const auto position = _mobility->GetPosition();

    MeshHeader header(_address, destination);
    packet.AddHeader(header);
    if (_send_callback) {
        _send_callback(this, position, packet);
    } else {
        NS_LOG_INFO("No send callback set, not sending packet");
    }
}

void MeshNetDevice::SetSendCallback(send_callback callback) {
    _send_callback = callback;
}

void MeshNetDevice::SetReceiveCallback(receive_callback callback) {
    NS_LOG_FUNCTION(this << static_cast<bool>(callback));
    _receive_callback = callback;
}

void MeshNetDevice::SetMobilityModel(ns3::Ptr<ns3::MobilityModel> mobility) {
    _mobility = mobility;
}

ns3::Ptr<ns3::MobilityModel> MeshNetDevice::GetMobilityModel() {
    return _mobility;
}

void MeshNetDevice::Receive(ns3::Packet packet) {
    NS_LOG_FUNCTION(this << packet);
    // Filter by address
    MeshHeader header;
    packet.PeekHeader(header);
    if (header.DestinationAddress() == _address
        || header.DestinationAddress() == IcaoAddress::Broadcast()) {
        NS_LOG_INFO("Received packet " << packet);
        if (_receive_callback) {
            _receive_callback(packet);
        } else {
            NS_LOG_INFO("No receive callback set, not forwarding packet up");
        }
    }
}

ns3::TypeId MeshNetDevice::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("MeshNetDevice")
        .SetParent<Object>()
        .AddConstructor<MeshNetDevice>();
    return id;
}
