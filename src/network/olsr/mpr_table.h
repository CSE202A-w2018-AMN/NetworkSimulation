#ifndef NETWORK_OLSR_MPR_TABLE_H
#define NETWORK_OLSR_MPR_TABLE_H
#include <ns3/nstime.h>
#include "address/icao_address.h"
#include <map>

namespace olsr {

/**
 * A table of neighbors that have recorded this node as a multipoint routing
 * neighbor
 */
class MprTable {
public:
    /**
     * An entry in a multipoint routing table
     */
    class Entry {
    private:
        /** The address of the neighbor */
        IcaoAddress _address;
        /** The time when this entry was last updated */
        ns3::Time _last_updated;
    public:
        Entry(IcaoAddress address);
        IcaoAddress Address() const;
        ns3::Time LastUpdated() const;
        void MarkSeen();
        friend std::ostream& operator << (std::ostream& stream, const Entry& entry);
    };

private:
    /** Table of entries */
    std::map<IcaoAddress, Entry> _table;
    /** Sequence number, updated on modification */
    std::uint8_t _sequence;
    /** Maximum time to keep entries after last seen */
    ns3::Time _ttl;

public:
    typedef std::map<IcaoAddress, Entry>::iterator iterator;
    typedef std::map<IcaoAddress, Entry>::const_iterator const_iterator;

    MprTable(ns3::Time ttl);

    inline std::size_t size() const {
        return _table.size();
    }
    inline iterator begin() {
        return _table.begin();
    }
    inline iterator end() {
        return _table.end();
    }
    inline const_iterator begin() const {
        return _table.begin();
    }
    inline const_iterator end() const {
        return _table.end();
    }
    inline iterator Find(IcaoAddress address) {
        return _table.find(address);
    }

    void Insert(IcaoAddress address);
    void RemoveExpired();

    inline std::uint8_t Sequence() const {
        return _sequence;
    }
    void IncrementSequence();
};

}

#endif
