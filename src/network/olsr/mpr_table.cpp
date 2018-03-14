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

MprTable::MprTable(ns3::Time ttl) :
    _ttl(ttl)
{
}

void MprTable::Insert(IcaoAddress address) {
    _table.insert(std::make_pair(address, Entry(address)));
}

void MprTable::RemoveExpired() {
    const auto now = ns3::Simulator::Now();
    for (auto iter = _table.begin(); iter != _table.end(); /* nothing */) {
        const auto updated = iter->second.LastUpdated();
        if (now - updated > _ttl) {
            // Remove
            const auto to_remove = iter;
            ++iter;
            _table.erase(to_remove);
        } else {
            ++iter;
        }
    }
}

}
