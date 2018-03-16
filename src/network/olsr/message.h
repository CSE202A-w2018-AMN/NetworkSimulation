#ifndef NETWORK_OLSR_MESSAGE_H
#define NETWORK_OLSR_MESSAGE_H

#include "address/icao_address.h"
#include "neighbor_table.h"
#include "mpr_table.h"
#include <vector>

namespace olsr {

/** OLSR message types */
enum class MessageType {
    /** An empty message */
    None,
    /** A hello message */
    Hello,
    /** A topology control message */
    TopologyControl,
    /** A data message */
    Data,
};

/**
 * Message tagged union
 */
class Message {
public:
    /** Creates an empty message */
    Message();
    /** Creates a message with the provided type */
    Message(MessageType type, std::uint8_t ttl = 0);

    /** Convenience constructor for a Data message */
    static Message Data(IcaoAddress origin, IcaoAddress destination, std::uint8_t ttl, std::uint16_t data_length);

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
    inline IcaoAddress Originator() const {
        return _originator;
    }
    inline void SetOriginator(IcaoAddress originator) {
        _originator = originator;
    }
    inline std::uint8_t Ttl() const {
        return _ttl;
    }
    inline void SetTtl(std::uint8_t ttl) {
        _ttl = ttl;
    }
    /** Decrements the TTL of this message if it is greater than zero */
    void DecrementTtl();

    IcaoAddress Origin() const {
        return _origin;
    }
    void SetOrigin(IcaoAddress origin) {
        _origin = origin;
    }
    IcaoAddress Destination() const {
        return _destination;
    }
    void SetDestination(IcaoAddress destination) {
        _destination = destination;
    }
    std::uint16_t DataLength() const {
        return _data_length;
    }
    void SetDataLength(std::uint16_t data_length) {
        _data_length = data_length;
    }
private:
    MessageType _type;
    /** Time to live */
    std::uint8_t _ttl;

    // Hello message
    /** Neighbors (used for Hello messages) */
    NeighborTable _neighbors;
    // TopologyControl message
    /** Originator address */
    IcaoAddress _originator;
    /** MPR selector table */
    MprTable _mpr_selector;

    // Data message
    /** Sender address */
    IcaoAddress _origin;
    /** Destination address */
    IcaoAddress _destination;
    /** Length of data, bytes */
    std::uint16_t _data_length;
};

}

#endif
