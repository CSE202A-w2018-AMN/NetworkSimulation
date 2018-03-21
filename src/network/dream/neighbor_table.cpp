#include "neighbor_table.h"
#include <ns3/simulator.h>

namespace dream {

// NeighborTableEntry

NeighborTableEntry::NeighborTableEntry(IcaoAddress address, const ns3::Vector& location) :
    _address(address),
    _location(location),
    _updated(ns3::Simulator::Now())
{
}

void NeighborTableEntry::MarkSeen() {
    _updated = ns3::Simulator::Now();
}

// NeighborTable

NeighborTable::NeighborTable(ns3::Time ttl) :
    _ttl(ttl)
{
}

void NeighborTable::RemoveExpired() {
    const auto now = ns3::Simulator::Now();
    for (auto iter = _table.begin(); iter != _table.end(); /* none */) {
        const auto& table_entry = iter->second;
        if (now - table_entry.LastUpdated() > _ttl) {
            // Delete
            // This invalidates the iterator to the removed element,
            // but not to other elements
            const auto to_remove = iter;
            iter = ++iter;
            _table.erase(to_remove);
        } else {
            iter = ++iter;
        }
    }
}

NeighborTable::iterator NeighborTable::Find(IcaoAddress address) {
    return _table.find(address);
}
NeighborTable::const_iterator NeighborTable::Find(IcaoAddress address) const {
    return _table.find(address);
}

NeighborTable::iterator NeighborTable::begin() {
    return _table.begin();
}
NeighborTable::iterator NeighborTable::end() {
    return _table.end();
}

NeighborTable::const_iterator NeighborTable::begin() const {
    return _table.begin();
}
NeighborTable::const_iterator NeighborTable::end() const {
    return _table.end();
}

std::size_t NeighborTable::size() const {
    return _table.size();
}
void NeighborTable::clear() {
    _table.clear();
}

void NeighborTable::Insert(const NeighborTableEntry& entry) {
    _table.insert(std::make_pair(entry.Address(), entry));
}

}
