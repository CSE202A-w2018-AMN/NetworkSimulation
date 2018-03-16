#include "message.h"
#include <stdexcept>

namespace olsr {

Message::Message() :
    Message(MessageType::None, 0)
{
}

Message::Message(MessageType type, std::uint8_t ttl) :
    _type(type),
    _ttl(ttl)
{
}

MessageType Message::Type() const {
    return _type;
}

void Message::SetType(MessageType type) {
    _type = type;
}

NeighborTable& Message::Neighbors() {
    return _neighbors;
}
const NeighborTable& Message::Neighbors() const {
    return _neighbors;
}

void Message::DecrementTtl() {
    if (_ttl > 0) {
        _ttl = _ttl - 1;
    }
}


}
