#include "header.h"
#include "util/bits.h"
#include "util/print_container.h"

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
    os << "OLSR, TTL " << _message.Ttl();
    switch (_message.Type()) {
    case MessageType::Hello:
        os << ", Hello, neighbors {";
        for (const auto& entry : _message.Neighbors()) {
            const auto& table_entry = entry.second;
            os << ' ' << table_entry.Address() << ':' << table_entry.State();
        }
        os << '}';
        break;
    case MessageType::TopologyControl:
        os << ", Topology control originating at "
            << _message.Originator() << ", sequence "
            << std::dec << _message.MprSelector().Sequence()
            << ", MPR selector " << print_container::print(_message.MprSelector());
        break;
    default:
        break;
    }
}

std::uint32_t Header::Deserialize(ns3::Buffer::Iterator start) {
    const auto ttl = start.ReadU8();
    _message.SetTtl(ttl);
    const auto type_key = start.ReadU8();
    switch (type_key) {
    case 1:
        _message.SetType(MessageType::Hello);
        return 2 + DeserializeHello(start);
    case 2:
        _message.SetType(MessageType::TopologyControl);
        return 2 + DeserializeTopologyControl(start);
    case 3:
        _message.SetType(MessageType::Data);
        return 2 + DeserializeData(start);
    case 0:
        _message.SetType(MessageType::None);
        // Nothing else
        return 2;
    default:
        throw std::runtime_error("Invalid message type");
    }
}

std::uint32_t Header::GetSerializedSize() const {
    switch (_message.Type()) {
    case MessageType::Hello:
        return 1 + 1 + 2 + 4 * _message.Neighbors().size();
    case MessageType::TopologyControl:
        return 1 + 1 + 3 + 1 + 2 + 3 * _message.MprSelector().size();
    case MessageType::Data:
        return 1 + 1 + 3 + 3 + 2;
    case MessageType::None:
        return 1 + 1;
    default:
        throw std::runtime_error("Invalid message type");
    }
}

void Header::Serialize(ns3::Buffer::Iterator start) const {
    start.WriteU8(_message.Ttl());
    switch (_message.Type()) {
    case MessageType::Hello:
        SerializeHello(start);
        break;
    case MessageType::TopologyControl:
        SerializeTopologyControl(start);
        break;
    case MessageType::Data:
        SerializeData(start);
        break;
    case MessageType::None:
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
    start.WriteU16(_message.Neighbors().size());
    for (const auto& entry : _message.Neighbors()) {
        const auto& table_entry = entry.second;
        bits::write_u24(&start, table_entry.Address().Value());
        start.WriteU8(static_cast<std::uint8_t>(table_entry.State()));
    }
}

std::uint32_t Header::DeserializeHello(ns3::Buffer::Iterator after_type) {
    // Clear old values
    _message.Neighbors().clear();

    const std::uint16_t neighbor_count = after_type.ReadU16();
    for (std::uint16_t i = 0; i < neighbor_count; i++) {
        const auto address = IcaoAddress(bits::read_u24(&after_type));
        const auto state = static_cast<LinkState>(after_type.ReadU8());
        _message.Neighbors().Insert(NeighborTableEntry(address, state));
    }

    return 2 + 4 * static_cast<std::uint32_t>(neighbor_count);
}

void Header::SerializeTopologyControl(ns3::Buffer::Iterator start) const {
    start.WriteU8(2);
    const auto& mpr_selector = _message.MprSelector();
    bits::write_u24(&start, _message.Originator().Value());
    start.WriteU8(mpr_selector.Sequence());
    start.WriteU16(static_cast<std::uint16_t>(mpr_selector.size()));
    for (const auto& entry : mpr_selector) {
        bits::write_u24(&start, entry.second.Address().Value());
    }
}

std::uint32_t Header::DeserializeTopologyControl(ns3::Buffer::Iterator after_type) {
    const auto originator = IcaoAddress(bits::read_u24(&after_type));
    _message.SetOriginator(originator);
    auto& mpr_selector = _message.MprSelector();
    mpr_selector.clear();
    const auto sequence = after_type.ReadU8();
    mpr_selector.SetSequence(sequence);
    const auto count = after_type.ReadU16();
    for (std::uint16_t i = 0; i < count; i++) {
        const auto address = bits::read_u24(&after_type);
        mpr_selector.Insert(IcaoAddress(address));
    }
    return 1 + 2 + 3 * static_cast<std::uint32_t>(count);
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
