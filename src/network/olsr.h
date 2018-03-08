#ifndef NETWORK_OLSR_H
#define NETWORK_OLSR_H

#include "device/mesh_net_device.h"
#include <ns3/packet.h>
#include "address/icao_address.h"

/**
 * An optimized link-state routing protocol implementation
 */
class Olsr {
public:

private:
    /** The network device used for communication */
    ns3::Ptr<MeshNetDevice> _net_device;

    /**
     * Called when the network device receives a packet
     *
     * The packet should include headers with address information.
     */
    void OnPacketReceived(ns3::Packet packet);

    /**
     * Sends a packet to a destination address
     */
    void SendPacket(ns3::Packet packet, IcaoAddress address);
};

#endif
