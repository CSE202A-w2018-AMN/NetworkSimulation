#include "header.h"
#include "util/bits.h"
#include <ns3/vector.h>

namespace dream {

namespace {

void write_vector(ns3::Buffer::Iterator* start, const ns3::Vector& vector) {
    // X, Y, Z
    start->Write(reinterpret_cast<const std::uint8_t*>(&vector.x), sizeof vector.x);
    start->Write(reinterpret_cast<const std::uint8_t*>(&vector.y), sizeof vector.y);
    start->Write(reinterpret_cast<const std::uint8_t*>(&vector.z), sizeof vector.z);
}

ns3::Vector read_vector(ns3::Buffer::Iterator* start) {
    ns3::Vector v;
    start->Read(reinterpret_cast<std::uint8_t*>(&v.x), sizeof v.x);
    start->Read(reinterpret_cast<std::uint8_t*>(&v.y), sizeof v.y);
    start->Read(reinterpret_cast<std::uint8_t*>(&v.z), sizeof v.z);
    return v;
}

void write_double(ns3::Buffer::Iterator* start, double v) {
    start->Write(reinterpret_cast<const std::uint8_t*>(&v), sizeof v);
}

double read_double(ns3::Buffer::Iterator* start) {
    double v;
    start->Read(reinterpret_cast<std::uint8_t*>(&v), sizeof v);
    return v;
}

}

Header::Header(const Message& message):
    _message(message)
{
}

ns3::TypeId Header::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("dream::Header")
        .SetParent<ns3::Header>()
        .AddConstructor<Header>();
    return id;
}

const Message& Header::GetMessage() const {
    return _message;
}
Message& Header::GetMessage() {
    return _message;
}

ns3::TypeId Header::GetInstanceTypeId() const {
    return GetTypeId();
}

void Header::Print(std::ostream& os) const {
    // Not currently implemented
}

std::uint32_t Header::Deserialize(ns3::Buffer::Iterator start) {
    const auto ttl = start.ReadU8();
    _message.SetTtl(ttl);
    const auto type_key = start.ReadU8();
    switch (type_key) {
    case 1:
        _message.SetType(Message::Type::Hello);
        return 2 + DeserializeHello(start);
    case 2:
        _message.SetType(Message::Type::Position);
        return 2 + DeserializePosition(start);
    case 3:
        _message.SetType(Message::Type::Data);
        return 2 + DeserializeData(start);
    case 0:
        _message.SetType(Message::Type::None);
        // Nothing else
        return 2;
    default:
        throw std::runtime_error("Invalid message type");
    }
}

std::uint32_t Header::GetSerializedSize() const {
    switch (_message.GetType()) {
    case Message::Type::Hello:
        return 1 + 1 + 24 + 24;
    case Message::Type::Position:
        return 1 + 1 + 3 + 24 + 24 + 8;
    case Message::Type::Data:
        return 1 + 1 + 3 + 3 + 2;
    case Message::Type::None:
        return 1 + 1;
    default:
        throw std::runtime_error("Invalid message type");
    }
}

void Header::Serialize(ns3::Buffer::Iterator start) const {
    start.WriteU8(_message.Ttl());
    switch (_message.GetType()) {
    case Message::Type::Hello:
        SerializeHello(start);
        break;
    case Message::Type::Position:
        SerializePosition(start);
        break;
    case Message::Type::Data:
        SerializeData(start);
        break;
    case Message::Type::None:
        SerializeNone(start);
        break;
    default:
        throw std::runtime_error("Invalid message type");
    }
}

void Header::SerializeNone(ns3::Buffer::Iterator start) const {
    // Type only
    start.WriteU8(0);
}

void Header::SerializeHello(ns3::Buffer::Iterator start) const {
    start.WriteU8(1);
    write_vector(&start, _message.Position());
    write_vector(&start, _message.Velocity());
}

std::uint32_t Header::DeserializeHello(ns3::Buffer::Iterator after_type) {
    _message.SetPosition(read_vector(&after_type));
    _message.SetVelocity(read_vector(&after_type));
    return 24 + 24;
}

void Header::SerializePosition(ns3::Buffer::Iterator start) const {
    start.WriteU8(2);
    bits::write_u24(&start, _message.Origin().Value());
    write_vector(&start, _message.Position());
    write_vector(&start, _message.Velocity());
    write_double(&start, _message.MaxDistance());
}

std::uint32_t Header::DeserializePosition(ns3::Buffer::Iterator after_type) {
    _message.SetOrigin(IcaoAddress(bits::read_u24(&after_type)));
    _message.SetPosition(read_vector(&after_type));
    _message.SetVelocity(read_vector(&after_type));
    _message.SetMaxDistance(read_double(&after_type));
    return 3 + 24 + 24 + 8;
}

void Header::SerializeData(ns3::Buffer::Iterator start) const {
    start.WriteU8(3);
    bits::write_u24(&start, _message.Origin().Value());
    bits::write_u24(&start, _message.Destination().Value());
    start.WriteU16(_message.DataLength());
}

std::uint32_t Header::DeserializeData(ns3::Buffer::Iterator after_type) {
    _message.SetOrigin(IcaoAddress(bits::read_u24(&after_type)));
    _message.SetDestination(IcaoAddress(bits::read_u24(&after_type)));
    const auto length = after_type.ReadU16();
    _message.SetDataLength(length);
    return 2;
}


}
