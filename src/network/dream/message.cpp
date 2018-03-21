#include "message.h"

namespace dream {

Message::Message() :
    Message(Type::None)
{
}

Message::Message(Type type, std::uint8_t ttl) :
    _type(type),
    _ttl(ttl),
    _data_length(0)
{
}

Message Message::DataMessage(IcaoAddress origin, IcaoAddress destination, std::uint8_t ttl, std::uint16_t data_length) {
    Message message(Type::Data, ttl);
    message.SetOrigin(origin);
    message.SetDestination(destination);
    message.SetDataLength(data_length);
    return message;
}

Message Message::HelloMessage(const ns3::Vector& position) {
    Message message(Type::Hello, 0);
    message.SetPosition(position);
    return message;
}
Message Message::PositionMessage(IcaoAddress origin, std::uint8_t ttl, const ns3::Vector& position, const ns3::Vector& velocity, double max_distance) {
    Message message(Type::Position, ttl);
    message.SetOrigin(origin);
    message.SetPosition(position);
    message.SetVelocity(velocity);
    message.SetMaxDistance(max_distance);
    return message;
}

void Message::DecrementTtl() {
    if (_ttl > 0) {
        _ttl -= 1;
    }
}

}
