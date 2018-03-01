#include "mesh_header.h"

namespace {

/**
 * Reads a 24-bit integer from the provided iterator in network byte order
 * and returns it
 */
std::uint32_t read_u24_ntoh(ns3::Buffer::Iterator* source) {
    std::uint32_t bits = 0;
    bits |= static_cast<std::uint32_t>(source->ReadU8()) << 16;
    bits |= static_cast<std::uint32_t>(source->ReadU8()) << 8;
    bits |= static_cast<std::uint32_t>(source->ReadU8());
    return bits;
}

/**
 * Writes a 24-bit integer to the provided iterator in network byte order
 */
void write_u24_ntoh(ns3::Buffer::Iterator* dest, std::uint32_t bits) {
    dest->WriteU8(static_cast<std::uint8_t>(bits >> 16));
    dest->WriteU8(static_cast<std::uint8_t>(bits >> 8));
    dest->WriteU8(static_cast<std::uint8_t>(bits));
}

}

MeshHeader::MeshHeader() :
    MeshHeader(IcaoAddress(), IcaoAddress())
{
}

MeshHeader::MeshHeader(IcaoAddress source, IcaoAddress destination) :
    _source(source),
    _destination(destination)
{
}

IcaoAddress MeshHeader::get_source_address() const {
    return _source;
}

IcaoAddress MeshHeader::get_destination_address() const {
    return _destination;
}

ns3::TypeId MeshHeader::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("MeshHeader")
        .SetParent<Header>()
        .AddConstructor<MeshHeader>();
    return id;
}

ns3::TypeId MeshHeader::GetInstanceTypeId() const {
    return GetTypeId();
}

void MeshHeader::Print(std::ostream& os) const {
    os << "Mesh " << _source << " => " << _destination;
}

std::uint32_t MeshHeader::Deserialize(ns3::Buffer::Iterator start) {
    _source = IcaoAddress(read_u24_ntoh(&start));
    _destination = IcaoAddress(read_u24_ntoh(&start));
    return 6;
}

std::uint32_t MeshHeader::GetSerializedSize() const {
    return 6;
}

void MeshHeader::Serialize(ns3::Buffer::Iterator start) const {
    write_u24_ntoh(&start, _source.Value());
    write_u24_ntoh(&start, _destination.Value());
}
