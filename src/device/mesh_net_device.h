#ifndef MESH_NET_DEVICE_H
#define MESH_NET_DEVICE_H

#include <ns3/object.h>
#include <ns3/packet.h>
#include <address/icao_address.h>

/**
 * A network device on an aircraft or ground station used for communication
 */
class MeshNetDevice : public ns3::Object {
public:
    /** Creates a nework device */
    MeshNetDevice();

    IcaoAddress get_address() const;
    void set_address(IcaoAddress address);

    /**
     * Sends a packet to the provided destination
     */
    void send(ns3::Ptr<ns3::Packet> packet, IcaoAddress destination);


    static ns3::TypeId GetTypeId();

private:
    /** The address of the associated aircraft */
    IcaoAddress _address;
};

#endif
