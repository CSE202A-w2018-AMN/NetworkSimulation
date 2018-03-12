#ifndef NETWORK_OLSR_NEIGHBOR_TABLE_H
#define NETWORK_OLSR_NEIGHBOR_TABLE_H
#include <map>
#include <set>
#include <ns3/nstime.h>
#include "address/icao_address.h"

namespace olsr {

/**
 * Link state values for each neighbor
 */
enum class LinkState {
    /** Have a unidirectional link from the neighbor to this node */
    Unidirectional,
    /** Have a bidirectional link to and from this neighbor */
    Bidirectional,
    /** Have a bidirectional link, and this neighbor is a multipoint relay */
    MultiPointRelay,
};

/**
 * An entry in a neighbor table
 */
class NeighborTableEntry {
private:
    /** The neighbor address */
    IcaoAddress _address;
    /** The link state */
    LinkState _state;
    /** The time when this entry was last updated */
    ns3::Time _updated;
public:
    NeighborTableEntry(IcaoAddress address, LinkState state);
    IcaoAddress Address() const;
    LinkState State() const;
    /** Returns the simulation time when this entry was updated */
    ns3::Time LastUpdated() const;

    /** Sets the link state and marks this entry as updated */
    void SetState(LinkState state);
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
    NeighborTable(ns3::Time ttl);
    /**
     * An iterator over pairs, where each pair contains an address and a
     * neighbor table entry
     */
    typedef std::map<IcaoAddress, NeighborTableEntry>::iterator iterator;

    /** Removes entries that have expired */
    void RemoveExpired();

    iterator Find(IcaoAddress address);
    void Insert(const NeighborTableEntry& entry);

    iterator begin();
    iterator end();

    /** Returns the addresses of neighbors with bidirectional or MPR links */
    std::set<IcaoAddress> Neighbors() const;
    /** Returns the addresses of neighbors with unidirectional links */
    std::set<IcaoAddress> UnidirectionalNeighbors() const;
};

}


#endif
