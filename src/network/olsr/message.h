#ifndef NETWORK_OLSR_MESSAGE_H
#define NETWORK_OLSR_MESSAGE_H

#include "address/icao_address.h"
#include <set>

namespace olsr {

/** OLSR message types */
enum class MessageType {
    /** An empty message */
    None,
    /** A hello message */
    Hello,
};

/**
 * Message tagged union
 */
class Message {
public:
    /** Creates an empty message */
    Message();
    /** Creates a message with the provided type */
    Message(MessageType type);

    MessageType Type() const;
    void SetType(MessageType type);

    std::set<IcaoAddress>& Neighbors();
    const std::set<IcaoAddress>& Neighbors() const;
    std::set<IcaoAddress>& UnidirectionalNeighbors();
    const std::set<IcaoAddress>& UnidirectionalNeighbors() const;
private:
    MessageType _type;

    // Hello message
    /** Neighbors with bidirectional links */
    std::set<IcaoAddress> _neighbors;
    /** Neighbors with unidirectional links */
    std::set<IcaoAddress> _unidirectional_neighbors;
};

}

#endif
