#ifndef NETWORK_DREAM_H
#define NETWORK_DREAM_H

#include "device/mesh_net_device.h"
#include "address/icao_address.h"
#include "network/network_protocol.h"
#include "message.h"
#include "routing_table.h"
#include "neighbor_table.h"
#include <ns3/packet.h>
#include <ns3/nstime.h>
#include <ostream>

namespace dream {


/**
 * A DREAM () protocol implementation
 */
class Dream : public NetworkProtocol {
public:
    typedef std::function<void(ns3::Packet)> receive_callback;


    Dream(ns3::Ptr<MeshNetDevice> net_device = ns3::Ptr<MeshNetDevice>());
    /**
     * Starts sending messages and performing other network operations
     */
    virtual void Start() override;

    /**
     * Sends a packet to the specified destination
     */
    virtual void Send(ns3::Packet packet, IcaoAddress destination) override;

    virtual void SetReceiveCallback(receive_callback callback) override;
    virtual void SetNetDevice(ns3::Ptr<MeshNetDevice> net_device) override;
    virtual void SetPacketRecorder(ns3::Ptr<PacketRecorder> recorder) override;

    static ns3::TypeId GetTypeId();

    inline const RoutingTable& Routing() const {
        return _routing;
    }

    inline const NeighborTable& Neighbors() const {
        return _neighbors;
    }

private:
    /** The network device used for communication */
    ns3::Ptr<MeshNetDevice> _net_device;

    /** Routing table */
    RoutingTable _routing;
    /** Neighbor table */
    NeighborTable _neighbors;

    /** Default TTL for sending messages */
    std::uint8_t _default_ttl;
    /** Data receive callback */
    receive_callback _receive_callback;

    /**
     * Interval between hello messages
     */
    ns3::Time _hello_interval;
    /** Interval between cleanups */
    ns3::Time _cleanup_interval;
    /** Interval between frequent position messages */
    ns3::Time _frequent_position_interval;
    /** Interval between infrequent position messages */
    ns3::Time _infrequent_position_interval;
    /** Frequent position message max distance */
    double _frequent_max_distance;
    /** Infrequent position message max distance */
    double _infrequent_max_distance;

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
     * Sends a packet
     *
     * This is similar to Send(), but it requires the packet to already have
     * a DREAM header.
     */
    void SendWithHeader(ns3::Packet packet, IcaoAddress destination);

    void HandleHello(IcaoAddress sender, const ns3::Vector& position);
    void HandlePosition(IcaoAddress sender, Message&& message);
    void HandleData(ns3::Packet packet, Message&& message);

    /** Sends a Position message with the specified maximum distance */
    void SendPosition(double max_distance);

    /** Event callback: Sends a hello message */
    void SendHello();
        /** Event callback: Sends an infrequent position message */
    void SendInfrequentPosition();
    /** Event callback: Sends a frequent position message */
    void SendFrequentPosition();

    /**
     * Event callback: Cleans up expired entries and recalculates routes
     */
    void Cleanup();
};

}

#endif
