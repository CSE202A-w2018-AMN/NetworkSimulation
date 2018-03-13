#include "message.h"
#include <stdexcept>

namespace olsr {

Message::Message() :
    Message(MessageType::None)
{
}

Message::Message(MessageType type) :
    _type(type)
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


}
