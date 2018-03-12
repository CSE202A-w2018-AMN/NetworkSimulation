#ifndef NETWORK_OLSR_H
#define NETWORK_OLSR_H

#include "device/mesh_net_device.h"
#include "address/icao_address.h"
#include <ns3/packet.h>
#include <ns3/nstime.h>
#include <set>

namespace olsr {

/**
 * An optimized link-state routing protocol implementation
 */
class Olsr : public ns3::Object {
public:
    Olsr(ns3::Ptr<MeshNetDevice> net_device = ns3::Ptr<MeshNetDevice>());
    /**
     * Starts sending hello messages and performing other network operations
     */
    void Start();

    void SetNetDevice(ns3::Ptr<MeshNetDevice> net_device);

    static ns3::TypeId GetTypeId();
private:
    /** The network device used for communication */
    ns3::Ptr<MeshNetDevice> _net_device;

    /**
     * Interval between hello messages
     */
    ns3::Time _hello_interval;

    /**
     * The addresses in the multipoint relay set
     *
     * Get a packet from a node not in the multipoint relay set:
     * Process the packet, but do not retransmit if it is broadcast
     *
     *
     */
    std::set<IcaoAddress> _multipoint_relays;
    /**
     * The neighbors of this node, with a bidirectional connection
     */
     std::set<IcaoAddress> _neighbors;
     /**
      * The neighbors that have been seen, but that do not have a confirmed
      * bidirectional connection
      */
     std::set<IcaoAddress> _unidirectional_neighbors;

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

    /**
     * Sends a Hello message with the current peers
     */
    void SendHello();

    /**
     * Handles a Hello message
     */
    void HandleHello(IcaoAddress sender, const std::set<IcaoAddress>& neighbors, const std::set<IcaoAddress>& unidirectional_neighbors);
};

}

#endif