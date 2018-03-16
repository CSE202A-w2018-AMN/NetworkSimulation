#ifndef NETWORK_OLSR_ROUTING_TABLE_H
#define NETWORK_OLSR_ROUTING_TABLE_H

#include "address/icao_address.h"
#include "util/value_iterator.h"
#include <cstdint>
#include <map>

namespace olsr {

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
    };
private:
    typedef std::map<IcaoAddress, Entry>::iterator underlying_iterator;
public:
    typedef util::ValueIterator<underlying_iterator> iterator;

    inline std::size_t size() const {
        return _table.size();
    }

    inline iterator begin() {
        return iterator(_table.begin());
    }
    inline iterator end() {
        return iterator(_table.end());
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
