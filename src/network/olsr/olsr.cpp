#include "olsr.h"
#include "header.h"

namespace olsr {

void Olsr::OnPacketReceived(ns3::Packet packet) {
    Header header;
    packet.RemoveHeader(header);
    if (header.GetMessage().Type() == MessageType::Hello) {
        HandleHello(header.GetMessage().Neighbors(), header.GetMessage().UnidirectionalNeighbors());
    }
}

void Olsr::SendPacket(ns3::Packet packet, IcaoAddress address) {
    _net_device->Send(packet, address);
}

void Olsr::SendHello() {
    auto packet = ns3::Packet();
    auto message = Message(MessageType::Hello);

    // Add neighbors
    message.Neighbors() = _neighbors;
    message.UnidirectionalNeighbors() = _unidirectional_neighbors;

    packet.AddHeader(Header(message));
    SendPacket(packet, IcaoAddress::Broadcast());
}

void Olsr::HandleHello(const std::set<IcaoAddress>& neighbors, const std::set<IcaoAddress>& unidirectional_neighbors) {

}

}
