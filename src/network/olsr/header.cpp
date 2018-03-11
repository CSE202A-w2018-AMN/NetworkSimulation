#include "header.h"
#include "util/bits.h"

namespace olsr {

Header::Header(const Message& message):
    _message(message)
{
}

ns3::TypeId Header::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("olsr::Header")
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
    os << "OLSR";
    switch (_message.Type()) {
    case MessageType::Hello:
        os << " Hello, neighbors {";
        for (const auto& neighbor : _message.Neighbors()) {
            os << ' ' << neighbor;
        }
        os << "}, unidirectional neighbors {";
        for (const auto& neighbor : _message.UnidirectionalNeighbors()) {
            os << ' ' << neighbor;
        }
        os << '}';
        break;
    default:
        break;
    }
}

std::uint32_t Header::Deserialize(ns3::Buffer::Iterator start) {
    const auto type_key = start.ReadU8();
    switch (type_key) {
    case 1:
        _message.SetType(MessageType::Hello);
        return 1 + DeserializeHello(start);
    case 0:
        _message.SetType(MessageType::None);
        // Nothing else
        return 1;
    default:
        throw std::runtime_error("Invalid message type");
    }
}

std::uint32_t Header::GetSerializedSize() const {
    switch (_message.Type()) {
    case MessageType::Hello:
        return 1 + 2 + 3 * _message.Neighbors().size() + 2 + 3 * _message.UnidirectionalNeighbors().size();
    case MessageType::None:
        return 1;
    default:
        throw std::runtime_error("Invalid message type");
    }
}

void Header::Serialize(ns3::Buffer::Iterator start) const {
    switch (_message.Type()) {
    case MessageType::Hello:
        SerializeHello(start);
    case MessageType::None:
        SerializeNone(start);
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
    start.WriteU16(_message.Neighbors().size());
    for (const auto& neighbor : _message.Neighbors()) {
        bits::write_u24(&start, neighbor.Value());
    }
    start.WriteU16(_message.UnidirectionalNeighbors().size());
    for (const auto& neighbor : _message.UnidirectionalNeighbors()) {
        bits::write_u24(&start, neighbor.Value());
    }
}

std::uint32_t Header::DeserializeHello(ns3::Buffer::Iterator after_type) {
    // Clear old values
    _message.Neighbors().clear();
    _message.UnidirectionalNeighbors().clear();

    const std::uint16_t neighbor_count = after_type.ReadU16();
    for (std::uint16_t i = 0; i < neighbor_count; i++) {
        const auto address = IcaoAddress(bits::read_u24(&after_type));
        _message.Neighbors().insert(address);
    }
    const std::uint16_t unidirectional_neighbor_count = after_type.ReadU16();
    for (std::uint16_t i = 0; i < unidirectional_neighbor_count; i++) {
        const auto address = IcaoAddress(bits::read_u24(&after_type));
        _message.UnidirectionalNeighbors().insert(address);
    }
    return 2 + 3 * static_cast<std::uint32_t>(neighbor_count) + 2 +
        3 * static_cast<std::uint32_t>(unidirectional_neighbor_count);
}

}
