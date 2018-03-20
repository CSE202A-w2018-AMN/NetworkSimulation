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
 * OLSR routing table
 */
class RoutingTable {
public:
    /** A routing table entry */
    class Entry {
    private:
        /** Destination address */
        IcaoAddress _destination;
        ns3::Vector _location;
        ns3::Vector _velocity;
        ns3::Time _last_time;

        /** Next hop address */
        IcaoAddress _next_hop;
        /** Estimated distance to destination */
        std::uint16_t _distance;
    public:
        Entry(IcaoAddress destination, IcaoAddress next_hop, std::uint16_t distance);
        inline IcaoAddress Destination() const {
            return _destination;
        }
        inline IcaoAddress NextHop() const {
            return _next_hop;
        }
        inline std::uint16_t Distance() const {
            return _distance;
        }
        inline ns3::Vector Location() const {
            return _location;
        }
        inline ns3::Vector Velocity() const {
            return _velocity;
        }
        inline ns3::Time Last_Time() const {
            return _last_time;
        }

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

private:
    /**
     * Table with a mapping from destination address to entry
     */
    std::map<IcaoAddress, Entry> _table;
};

}

#endif
