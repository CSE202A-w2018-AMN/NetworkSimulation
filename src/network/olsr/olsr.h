#ifndef NETWORK_OLSR_H
#define NETWORK_OLSR_H

#include "device/mesh_net_device.h"
#include "address/icao_address.h"
#include "neighbor_table.h"
#include "mpr_table.h"
#include "topology_table.h"
#include "routing_table.h"
#include <ns3/packet.h>
#include <ns3/nstime.h>
#include <set>
#include <ostream>

namespace olsr {

class Message;

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

    /**
     * Sends a packet to the specified destination
     */
    void Send(ns3::Packet packet, IcaoAddress destination);

    void SetNetDevice(ns3::Ptr<MeshNetDevice> net_device);

    static ns3::TypeId GetTypeId();

    inline const NeighborTable& Neighbors() const {
        return _neighbors;
    }
    inline const MprTable& MprSelector() const {
        return _mpr_selector;
    }
    inline const RoutingTable& Routing() const {
        return _routing;
    }

    /** A wrapper that dumps the state of an OLSR instance */
    class DumpState {
    private:
        const Olsr& _olsr;
    public:
        DumpState(const Olsr& olsr);
        friend std::ostream& operator << (std::ostream& stream, const DumpState& state);
    };
private:
    /** The network device used for communication */
    ns3::Ptr<MeshNetDevice> _net_device;

    /**
     * Interval between hello messages
     */
    ns3::Time _hello_interval;
    /**
     * Interval between topology control messages
     */
    ns3::Time _topology_control_interval;
    /**
     * Default TTL to use when sending non-local messages
     */
    std::uint8_t _default_ttl;

    /** Neighbor table */
    NeighborTable _neighbors;
    /** Table of neighbors that consider this node in their multipoint relay sets */
    MprTable _mpr_selector;
    /** Topology table */
    TopologyTable _topology;
    /** Routing table */
    RoutingTable _routing;

    /**
     * Called when the network device receives a packet
     *
     * The packet should include headers with address information.
     */
    void OnPacketReceived(ns3::Packet packet);

    /**
     * Sends a packet to a destination address
     *
     * Unlike Send(), this does not do any routing. It simply forwards the
     * packet to the network interface.
     */
    void SendPacket(ns3::Packet packet, IcaoAddress address);

    /**
     * Sends a Hello message with the current peers
     */
    void SendHello();

    /**
     * Sends a topology control message
     */
    void SendTopologyControl();

    /**
     * Handles a Hello message
     */
    void HandleHello(IcaoAddress sender, const NeighborTable& neighbors);
    void UpdateNeighbors(IcaoAddress sender, const NeighborTable& sender_neighbors);
    void UpdateMprSelector(IcaoAddress sender, const NeighborTable& sender_neighbors);

    void HandleTopologyControl(IcaoAddress sender, Message&& message);
};

}

#endif
