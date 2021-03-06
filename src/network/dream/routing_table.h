#ifndef NETWORK_DREAM_ROUTING_TABLE_H
#define NETWORK_DREAM_ROUTING_TABLE_H

#include "address/icao_address.h"
#include "util/value_iterator.h"
#include <ns3/nstime.h>
#include <ns3/vector.h>
#include <cstdint>
#include <map>

namespace dream {

/**
 * DREAM routing table
 *
 * Stores the position and velocity of every other node in the network
 */
class RoutingTable {
public:
    /** A routing table entry */
    class Entry {
    private:
        /** Destination address */
        IcaoAddress _destination;
        /** Location of destination when last updated, m */
        ns3::Vector _location;
        /** Velocity of destination when last updated, m/s */
        ns3::Vector _velocity;
        /** Time when last updated */
        ns3::Time _last_time;
    public:
        Entry(IcaoAddress destination, const ns3::Vector& location, const ns3::Vector& velocity);
        inline IcaoAddress Destination() const {
            return _destination;
        }
        inline ns3::Vector Location() const {
            return _location;
        }
        inline void SetLocation(const ns3::Vector& location) {
            _location = location;
        }
        inline ns3::Vector Velocity() const {
            return _velocity;
        }
        inline void SetVelocity(const ns3::Vector& velocity) {
            _velocity = velocity;
        }
        inline ns3::Time LastTime() const {
            return _last_time;
        }
        /** Sets the last updated time to now */
        void MarkSeen();

    };

    /** An adapter that prints the routing table to a stream */
    class PrintTable {
    private:
        const RoutingTable& _table;
    public:
        PrintTable(const RoutingTable& table);
        friend std::ostream& operator << (std::ostream& stream, const PrintTable& pt);
    };
private:
    typedef std::map<IcaoAddress, Entry>::iterator underlying_iterator;
    typedef std::map<IcaoAddress, Entry>::const_iterator underlying_const_iterator;
public:
    typedef util::ValueIterator<underlying_iterator> iterator;
    typedef util::ConstValueIterator<underlying_const_iterator> const_iterator;

    RoutingTable(const ns3::Time& ttl);

    inline std::size_t size() const {
        return _table.size();
    }

    inline iterator begin() {
        return iterator(_table.begin());
    }
    inline iterator end() {
        return iterator(_table.end());
    }
    inline const_iterator begin() const {
        return const_iterator(_table.begin());
    }
    inline const_iterator end() const {
        return const_iterator(_table.end());
    }
    inline void clear() {
        _table.clear();
    }

    iterator Find(IcaoAddress destination);
    void Insert(Entry entry);
    /** Removes old entries */
    void RemoveExpired();

private:
    /**
     * Table with a mapping from destination address to entry
     */
    std::map<IcaoAddress, Entry> _table;
    /** Time since last update threshold for deleting old entries */
    ns3::Time _ttl;
};

}

#endif
