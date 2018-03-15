#ifndef NETWORK_OLSR_MESSAGE_H
#define NETWORK_OLSR_MESSAGE_H

#include "address/icao_address.h"
#include "neighbor_table.h"
#include "mpr_table.h"
#include <set>

namespace olsr {

/** OLSR message types */
enum class MessageType {
    /** An empty message */
    None,
    /** A hello message */
    Hello,
    /** A topology control message */
    TopologyControl,
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

    NeighborTable& Neighbors();
    const NeighborTable& Neighbors() const;
    inline MprTable& MprSelector() {
        return _mpr_selector;
    }
    inline const MprTable& MprSelector() const {
        return _mpr_selector;
    }
private:
    MessageType _type;

    // Hello message
    /** Neighbors (used for Hello messages) */
    NeighborTable _neighbors;
    // TopologyControl message
    MprTable _mpr_selector;
};

}

#endif
