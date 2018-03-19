#ifndef NETWORK_OLSR_H
#define NETWORK_OLSR_H

#include "device/mesh_net_device.h"
#include "address/icao_address.h"
#include "network/olsr/routing_table.h"
#include <ns3/packet.h>
#include <ns3/nstime.h>
#include <ostream>

namespace dream {


/**
 * A DREAM () protocol implementation
 */
class Dream : public ns3::Object {
public:
    Dream(ns3::Ptr<MeshNetDevice> net_device = ns3::Ptr<MeshNetDevice>());
    /**
     * Starts sending messages and performing other network operations
     */
    void Start();

    /**
     * Sends a packet to the specified destination
     */
    void Send(ns3::Packet packet, IcaoAddress destination);

    void SetNetDevice(ns3::Ptr<MeshNetDevice> net_device);

    static ns3::TypeId GetTypeId();

    inline const RoutingTable& Routing() const {
        return _routing;
    }

private:
    /** The network device used for communication */
    ns3::Ptr<MeshNetDevice> _net_device;

    /** Routing table */
    RoutingTable _routing;

    /**
     * Interval between hello messages
     */
    // ns3::Time _hello_interval;

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
};

}

#endif
