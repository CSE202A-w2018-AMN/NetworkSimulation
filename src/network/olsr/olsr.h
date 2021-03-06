#ifndef NETWORK_OLSR_H
#define NETWORK_OLSR_H

#include "device/mesh_net_device.h"
#include "address/icao_address.h"
#include "neighbor_table.h"
#include "mpr_table.h"
#include "topology_table.h"
#include "routing_table.h"
#include "packet_recorder/packet_recorder.h"
#include "network/network_protocol.h"
#include <ns3/packet.h>
#include <ns3/nstime.h>
#include <functional>
#include <ostream>
#include <memory>

namespace olsr {

class Message;

/**
 * An optimized link-state routing protocol implementation
 */
class Olsr : public NetworkProtocol {
public:
    typedef std::function<void(ns3::Packet)> receive_callback;

    Olsr(ns3::Ptr<MeshNetDevice> net_device = ns3::Ptr<MeshNetDevice>());
    /**
     * Starts sending hello messages and performing other network operations
     */
    virtual void Start() override;

    /**
     * Sends a packet to the specified destination
     */
    virtual void Send(ns3::Packet packet, IcaoAddress destination) override;

    virtual void SetReceiveCallback(receive_callback callback) override;

    virtual void SetNetDevice(ns3::Ptr<MeshNetDevice> net_device) override;

    IcaoAddress Address() const;

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
    inline const TopologyTable& Topology() const {
        return _topology;
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
    /** The packet recorder */
    ns3::Ptr<PacketRecorder> _packet_recorder;

    /**
     * Interval between hello messages
     */
    ns3::Time _hello_interval;
    /**
     * Interval between topology control messages
     */
    ns3::Time _topology_control_interval;
    /** Interval between Cleanup() calls */
    ns3::Time _cleanup_interval;
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

    /** Data receive callback */
    receive_callback _receive_callback;

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
    void SendPacket(ns3::Packet packet, IcaoAddress destination);

    /**
     * Sends a Hello message with the current peers
     */
    void SendHello();

    /**
     * Sends a topology control message
     */
    void SendTopologyControl();

    /**
     * Sends a packet to all of this node's multipoint relay neighbors
     * @param packet the packet to send, with an OLSR header already present
     */
    void SendMultipointRelay(ns3::Packet packet);

    /**
     * Sends a packet with an OLSR header already installed
     */
    void SendWithHeader(ns3::Packet packet, IcaoAddress destination);

    /**
     * Cleans up expired entries and recalculates routes
     */
    void Cleanup();

    /**
     * Handles a Hello message
     */
    void HandleHello(IcaoAddress sender, const NeighborTable& neighbors);
    void UpdateNeighbors(IcaoAddress sender, const NeighborTable& sender_neighbors);
    void UpdateMprSelector(IcaoAddress sender, const NeighborTable& sender_neighbors);

    void HandleTopologyControl(IcaoAddress sender, Message&& message);
    void HandleData(ns3::Packet packet, Message&& message);
};

}

#endif
