#include "mesh_header.h"
#include "util/bits.h"

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
    _source = IcaoAddress(bits::read_u24(&start));
    _destination = IcaoAddress(bits::read_u24(&start));
    return 6;
}

std::uint32_t MeshHeader::GetSerializedSize() const {
    return 6;
}

void MeshHeader::Serialize(ns3::Buffer::Iterator start) const {
    bits::write_u24(&start, _source.Value());
    bits::write_u24(&start, _destination.Value());
}
