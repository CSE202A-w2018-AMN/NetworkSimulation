#ifndef NETWORK_OLSR_TOPOLOGY_TABLE_H
#define NETWORK_OLSR_TOPOLOGY_TABLE_H
#include "address/icao_address.h"
#include "util/value_iterator.h"
#include <ns3/nstime.h>
#include <cstdint>
#include <map>

namespace olsr {

class TopologyTable {
public:
    class Entry {
    private:
        /** A possible destination */
        IcaoAddress _destination;
        /** The last hop on a route to the destination */
        IcaoAddress _last_hop;
        /** Sequence number of the topology control message used for this entry */
        std::uint8_t _sequence;
        /** The time when this entry was last updated */
        ns3::Time _updated;
    public:
        Entry(IcaoAddress destination, IcaoAddress last_hop, std::uint8_t sequence);
        inline IcaoAddress Destination() const {
            return _destination;
        }
        inline IcaoAddress LastHop() const {
            return _last_hop;
        }
        inline void SetLastHop(IcaoAddress last_hop) {
            _last_hop = last_hop;
        }
        inline std::uint8_t Sequence() const {
            return _sequence;
        }
        inline ns3::Time Updated() const {
            return _updated;
        }
        void MarkSeen();
    };
private:
    typedef std::map<IcaoAddress, Entry>::iterator underlying_iterator;
    typedef std::map<IcaoAddress, Entry>::const_iterator underlying_const_iterator;
public:

    typedef util::ValueIterator<underlying_iterator> iterator;
    typedef util::ConstValueIterator<underlying_const_iterator> const_iterator;

    TopologyTable(ns3::Time ttl = ns3::Minutes(5));

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
    inline std::size_t size() const {
        return _table.size();
    }

    iterator Find(IcaoAddress destination);
    void Insert(Entry entry);
    void Remove(iterator position);

    void RemoveExpired();

private:
    /** Maps from destination address to Entry */
    std::map<IcaoAddress, Entry> _table;
    /** Expiration time for entries */
    ns3::Time _ttl;
};

}

#endif
