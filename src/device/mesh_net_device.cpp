#include "mesh_net_device.h"
#include "header/mesh_header.h"
#include <ns3/log.h>

NS_LOG_COMPONENT_DEFINE("MeshNetDevice");

MeshNetDevice::MeshNetDevice() {}

IcaoAddress MeshNetDevice::get_address() const {
    return _address;
}

void MeshNetDevice::set_address(IcaoAddress address) {
    _address = address;
}

void MeshNetDevice::send(ns3::Ptr<ns3::Packet> packet, IcaoAddress destination) {
    NS_LOG_INFO("Sending " << packet->GetSize() << " bytes " << _address << " => " << destination);
    MeshHeader header(_address, destination);
    packet->AddHeader(header);
    // TODO: Transfer to ether
}

ns3::TypeId MeshNetDevice::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("MeshNetDevice")
        .SetParent<Object>()
        .AddConstructor<MeshNetDevice>();
    return id;
}
