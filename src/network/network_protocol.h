#ifndef NETWORK_NETWORK_PROTOCOL_H
#define NETWORK_NETWORK_PROTOCOL_H
#include <functional>
#include <ns3/object.h>
#include <ns3/packet.h>
#include "address/icao_address.h"

// Forward-declare
class MeshNetDevice;
class PacketRecorder;

/**
 * Base class for network protocols
 */
class NetworkProtocol : public ns3::Object {
public:
    /** Receive callback type */
    typedef std::function<void(ns3::Packet)> receive_callback;
    /**
     * Starts sending hello messages and performing other network operations
     */
    virtual void Start() = 0;

    /**
     * Sends a packet to the specified destination
     */
    virtual void Send(ns3::Packet packet, IcaoAddress destination) = 0;

    /**
     * Sets the callback to be notified when a data packet is received
     */
    virtual void SetReceiveCallback(receive_callback callback) = 0;

    /**
     * Sets the network device
     */
    virtual void SetNetDevice(ns3::Ptr<MeshNetDevice> net_device) = 0;
    /**
     * Sets the packet recorder
     */
    virtual void SetPacketRecorder(ns3::Ptr<PacketRecorder> recorder) = 0;

    /** Empty destructor */
    virtual ~NetworkProtocol() = default;

    static ns3::TypeId GetTypeId();
};

#endif
