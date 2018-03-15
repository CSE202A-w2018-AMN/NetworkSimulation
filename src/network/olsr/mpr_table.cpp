#include "mpr_table.h"
#include <ns3/simulator.h>

namespace olsr {

MprTable::Entry::Entry(IcaoAddress address) :
    _address(address),
    _last_updated(ns3::Simulator::Now())
{
}

IcaoAddress MprTable::Entry::Address() const {
    return _address;
}

ns3::Time MprTable::Entry::LastUpdated() const {
    return _last_updated;
}

void MprTable::Entry::MarkSeen() {
    _last_updated = ns3::Simulator::Now();
}

std::ostream& operator << (std::ostream& stream, const MprTable::Entry& entry) {
    return stream << entry._address;
}

MprTable::MprTable(ns3::Time ttl) :
    _sequence(0),
    _ttl(ttl)
{
}

void MprTable::Insert(IcaoAddress address) {
    _table.insert(std::make_pair(address, Entry(address)));
}

void MprTable::clear() {
    _table.clear();
}

void MprTable::RemoveExpired() {
    const auto now = ns3::Simulator::Now();
    auto any_removed = false;
    for (auto iter = _table.begin(); iter != _table.end(); /* nothing */) {
        const auto updated = iter->second.LastUpdated();
        if (now - updated > _ttl) {
            // Remove
            const auto to_remove = iter;
            ++iter;
            _table.erase(to_remove);
            any_removed = true;
        } else {
            ++iter;
        }
    }
    if (any_removed) {
        IncrementSequence();
    }
}

void MprTable::IncrementSequence() {
    _sequence += 1;
}

std::ostream& operator << (std::ostream& stream, const std::pair<IcaoAddress, MprTable::Entry>& entry) {
    return stream << entry.second;
}

}
