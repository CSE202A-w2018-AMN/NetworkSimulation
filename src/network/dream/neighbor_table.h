#ifndef NETWORK_DREAM_NEIGHBOR_TABLE_H
#define NETWORK_DREAM_NEIGHBOR_TABLE_H
#include <map>
#include <set>
#include <ostream>
#include <ns3/nstime.h>
#include <ns3/vector.h>
#include "address/icao_address.h"

namespace dream {

/**
 * An entry in a neighbor table
 */
class NeighborTableEntry {
private:
    /** The neighbor address */
    IcaoAddress _address;
    /** The location */
    ns3::Vector _location;
    /** The time when this entry was last updated */
    ns3::Time _updated;
public:
    NeighborTableEntry(IcaoAddress address, const ns3::Vector& location);
    inline IcaoAddress Address() const {
        return _address;
    }
    inline ns3::Vector Location() const {
        return _location;
    }
    inline void SetLocation(const ns3::Vector& location) {
        _location = location;
    }
    /** Returns the simulation time when this entry was updated */
    inline ns3::Time LastUpdated() const {
        return _updated;
    }
    /** Marks this entry as updated */
    void MarkSeen();
};

class NeighborTable {
private:
    /** The table of entries */
    std::map<IcaoAddress, NeighborTableEntry> _table;
    /** The time before entries expire */
    ns3::Time _ttl;
public:
    NeighborTable(ns3::Time ttl = ns3::Time());
    /**
     * An iterator over pairs, where each pair contains an address and a
     * neighbor table entry
     */
    typedef std::map<IcaoAddress, NeighborTableEntry>::iterator iterator;
    typedef std::map<IcaoAddress, NeighborTableEntry>::const_iterator const_iterator;

    /** Removes entries that have expired */
    void RemoveExpired();

    iterator Find(IcaoAddress address);
    const_iterator Find(IcaoAddress address) const;
    void Insert(const NeighborTableEntry& entry);

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    std::size_t size() const;
    void clear();
};

}


#endif
