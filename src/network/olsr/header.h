#ifndef NETWORK_OLSR_HEADER_H
#define NETWORK_OLSR_HEADER_H

#include <ns3/header.h>
#include "message.h"

namespace olsr {

/**
 * An OLSR header
 *
 * Header format:
 * 8-bit time to live
 * 8-bit message type (None = 0, Hello = 1, TopologyControl = 2, Data = 3)
 * Message-type-specific data
 *
 * None message data: (empty)
 *
 * Hello message data:
 * Number of neighbors, 2 bytes
 * For each neighbor:
 *     * Address, 3 bytes
 *     * Status, 1 byte (1 = unidirectional, 2 = bidirectional, 3 = multipoint relay)
 *
 * TopologyControl message data:
 * * Originator address, 3 bytes
 * * MPR selector sequence number, 1 byte
 * * Number of MPR selector addresses, 1 byte
 * * For each MPR selector:
 *     * Address, 3 bytes
 *
 * Data message data:
 * * Origin address, 3 bytes
 * * Destination address, 3 bytes
 * * Data length, 2 bytes
 * (Data are in the packet, outside this header)
 */
class Header : public ns3::Header {
public:
    Header() = default;
    Header(const Message& message);

    const Message& GetMessage() const;
    Message& GetMessage();

    static ns3::TypeId GetTypeId();
    virtual ns3::TypeId GetInstanceTypeId() const override;
    virtual void Print(std::ostream& os) const override;

    virtual std::uint32_t Deserialize(ns3::Buffer::Iterator start) override;
    virtual std::uint32_t GetSerializedSize() const override;
    virtual void Serialize(ns3::Buffer::Iterator start) const override;

private:
    /* The message in this header */
    Message _message;

    void SerializeNone(ns3::Buffer::Iterator start) const;
    void SerializeHello(ns3::Buffer::Iterator start) const;
    void SerializeTopologyControl(ns3::Buffer::Iterator start) const;
    void SerializeData(ns3::Buffer::Iterator start) const;

    std::uint32_t DeserializeHello(ns3::Buffer::Iterator after_type);
    std::uint32_t DeserializeTopologyControl(ns3::Buffer::Iterator after_type);
    std::uint32_t DeserializeData(ns3::Buffer::Iterator after_type);
};

}

#endif
