#ifndef MESH_HEADER_H
#define MESH_HEADER_H

#include <ns3/header.h>
#include "address/icao_address.h"

/**
 * A mesh packet header
 *
 *
 * Header format:
 * Source address, 24 bits, network byte order
 * Destination address, 24 bits, network byte order
 */
class MeshHeader : public ns3::Header {
public:
    MeshHeader();
    MeshHeader(IcaoAddress source, IcaoAddress destination);
    virtual ~MeshHeader() = default;

    IcaoAddress get_source_address() const;
    IcaoAddress get_destination_address() const;

    static ns3::TypeId GetTypeId();
    virtual ns3::TypeId GetInstanceTypeId() const override;
    virtual void Print(std::ostream& os) const override;

    virtual std::uint32_t Deserialize(ns3::Buffer::Iterator start) override;
    virtual std::uint32_t GetSerializedSize() const override;
    virtual void Serialize(ns3::Buffer::Iterator start) const override;

private:
    IcaoAddress _source;
    IcaoAddress _destination;
};

#endif
