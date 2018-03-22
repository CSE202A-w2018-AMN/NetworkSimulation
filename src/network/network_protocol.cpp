#include "network_protocol.h"

ns3::TypeId NetworkProtocol::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("NetworkProtocol")
        .SetParent<Object>();
    return id;
}

void NetworkProtocol::SetPacketRecorder(ns3::Ptr<PacketRecorder> packet_recorder) {
    _packet_recorder = packet_recorder;
}


void NetworkProtocol::RecordPacketSent(std::uint64_t id, PacketRecorder::PacketType type) {
    if (_packet_recorder) {
        _packet_recorder->RecordPacketSent(id, type);
    }
}
void NetworkProtocol::RecordPacketReceived(std::uint64_t id) {
    if (_packet_recorder) {
        _packet_recorder->RecordPacketReceived(id);
    }
}
