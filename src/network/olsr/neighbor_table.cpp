#include "neighbor_table.h"
#include <ns3/simulator.h>

namespace olsr {

// NeighborTableEntry

NeighborTableEntry::NeighborTableEntry(IcaoAddress address, LinkState state) :
    _address(address),
    _state(state),
    _updated(ns3::Simulator::Now())
{
}

IcaoAddress NeighborTableEntry::Address() const {
    return _address;
}
LinkState NeighborTableEntry::State() const {
    return _state;
}
void NeighborTableEntry::SetState(LinkState state) {
    _state = state;
    _updated = ns3::Simulator::Now();
}
void NeighborTableEntry::MarkSeen() {
    _updated = ns3::Simulator::Now();
}

ns3::Time NeighborTableEntry::LastUpdated () const {
    return _updated;
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

NeighborTable::iterator NeighborTable::begin() {
    return _table.begin();
}
NeighborTable::iterator NeighborTable::end() {
    return _table.end();
}

void NeighborTable::Insert(const NeighborTableEntry& entry) {
    _table.insert(std::make_pair(entry.Address(), entry));
}

std::set<IcaoAddress> NeighborTable::Neighbors() const {
    std::set<IcaoAddress> neighbors;
    for (const auto& entry : _table) {
        const NeighborTableEntry& neighbor_entry = entry.second;
        if (neighbor_entry.State() == LinkState::Bidirectional
            || neighbor_entry.State() == LinkState::MultiPointRelay) {
            neighbors.insert(neighbor_entry.Address());
        }
    }
    return neighbors;
}

std::set<IcaoAddress> NeighborTable::UnidirectionalNeighbors() const {
    std::set<IcaoAddress> neighbors;
    for (const auto& entry : _table) {
        const NeighborTableEntry& neighbor_entry = entry.second;
        if (neighbor_entry.State() == LinkState::Unidirectional) {
            neighbors.insert(neighbor_entry.Address());
        }
    }
    return neighbors;
}

}
