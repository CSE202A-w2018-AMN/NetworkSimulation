#ifndef NETWORK_DREAM_HEADER_H
#define NETWORK_DREAM_HEADER_H

#include <ns3/header.h>
#include "message.h"

namespace dream {

/**
 * A DREAM header
 *
 * Header format:
 * 8-bit time to live
 * 8-bit message type (None = 0, Hello = 1, Position = 2, Data = 3)
 * Message-type-specific data
 *
 * None message data: (empty)
 *
 * Hello message data:
 * Originator address, 3 bytes
 * Originator position, 24 bytes
 * Originator velocity, 24 bytes
 *
 * Position message data:
 * Originator address, 3 bytes
 * Originator position, 24 bytes
 * Originator velocity, 24 bytes
 * Maximum distance, 8 bytes
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
    void SerializePosition(ns3::Buffer::Iterator start) const;
    void SerializeData(ns3::Buffer::Iterator start) const;

    std::uint32_t DeserializeHello(ns3::Buffer::Iterator after_type);
    std::uint32_t DeserializePosition(ns3::Buffer::Iterator after_type);
    std::uint32_t DeserializeData(ns3::Buffer::Iterator after_type);
};

}

#endif
