#ifndef MESH_NET_DEVICE_H
#define MESH_NET_DEVICE_H

#include <functional>

#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/mobility-model.h>
#include <address/icao_address.h>

/**
 * A network device on an aircraft or ground station used for communication
 */
class MeshNetDevice : public ns3::Object {
public:

    /**
     * Packet send callback type
     * @param position the position of the sender
     * @param packet the packet that was sent, including mesh headers
     */
    typedef std::function<void (const ns3::Vector& position, ns3::Packet packet)> send_callback;

    /** Creates a nework device */
    MeshNetDevice();

    IcaoAddress GetAddress() const;
    void SetAddress(IcaoAddress address);

    /**
     * Sends a packet to the provided destination
     * @param position the current position of the node
     * @param packet the packet to send
     * @param destination the address to send to
     */
    void Send(ns3::Packet packet, IcaoAddress destination);

    /**
     * Receives a packet from the ether
     *
     * The packet should have a mesh header.
     */
    void Receive(ns3::Packet packet);

    /**
     * Sets the callback that will be notified when Send is called
     *
     * The callback will be given a packet with headers already added.
     */
    void SetSendCallback(send_callback callback);
    void SetMobilityModel(ns3::Ptr<ns3::MobilityModel> mobility);

    static ns3::TypeId GetTypeId();

private:
    /** The address of the associated aircraft */
    IcaoAddress _address;

    /** The send callback */
    send_callback _send_callback;

    /**
     * The mobility model of the connected node
     */
    ns3::Ptr<ns3::MobilityModel> _mobility;
};

#endif
