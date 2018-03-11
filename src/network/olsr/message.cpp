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

std::set<IcaoAddress>& Message::Neighbors() {
    return _neighbors;
}
const std::set<IcaoAddress>& Message::Neighbors() const {
    return _neighbors;
}

std::set<IcaoAddress>& Message::UnidirectionalNeighbors() {
    return _unidirectional_neighbors;
}
const std::set<IcaoAddress>& Message::UnidirectionalNeighbors() const {
    return _unidirectional_neighbors;
}

}
