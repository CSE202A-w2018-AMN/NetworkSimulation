#ifndef NETWORK_DREAM_MESSAGE_H
#define NETWORK_DREAM_MESSAGE_H
#include <cstdint>
#include <ostream>
#include <ns3/vector.h>
#include "address/icao_address.h"

namespace dream {

/**
 * A DREAM message
 */
class Message {
public:
    enum class Type {
        /** An empty message */
        None,
        /** A hello message used to find neighbors */
        Hello,
        /** A position message */
        Position,
        /** A data message */
        Data,
    };
    /** Creates an empty message with type None */
    Message();
    /** Creates a message with the provided type and TTL */
    Message(Type type, std::uint8_t ttl = 0);
    /** Convenience constructor for a Data message */
    static Message DataMessage(IcaoAddress origin, IcaoAddress destination, std::uint8_t ttl, std::uint16_t data_length);
    /** Convenience constructor for a Hello message */
    static Message HelloMessage(const ns3::Vector& position);
    /** Convenience constructor for a Position message */
    static Message PositionMessage(IcaoAddress origin, std::uint8_t ttl, const ns3::Vector& position, const ns3::Vector& velocity, double max_distance);

    inline Type GetType() const {
        return _type;
    }
    inline void SetType(Type type) {
        _type = type;
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

    ns3::Vector Position() const {
        return _position;
    }
    void SetPosition(const ns3::Vector& position) {
        _position = position;
    }
    ns3::Vector Velocity() const {
        return _velocity;
    }
    void SetVelocity(const ns3::Vector& velocity) {
        _velocity = velocity;
    }
    double MaxDistance() const {
        return _max_distance;
    }
    void SetMaxDistance(double max_distance) {
        _max_distance = max_distance;
    }

private:
    /** The type of this message */
    Type _type;
    /** Time to live */
    std::uint8_t _ttl;
    /** The origin address */
    IcaoAddress _origin;
    /** The destination address */
    IcaoAddress _destination;
    /** Length of data, bytes */
    std::uint16_t _data_length;
    /** Origin position, m */
    ns3::Vector _position;
    /** Origin velocity, m/s */
    ns3::Vector _velocity;
    /** Maximum distance, m */
    double _max_distance;
};

std::ostream& operator << (std::ostream& stream, const Message::Type& type);

}

#endif
