#include "topology_table.h"
#include <ns3/simulator.h>

namespace olsr {

TopologyTable::Entry::Entry(IcaoAddress destination, IcaoAddress last_hop, std::uint8_t sequence) :
    _destination(destination),
    _last_hop(last_hop),
    _sequence(sequence),
    _updated(ns3::Simulator::Now())
{
}

void TopologyTable::Entry::MarkSeen() {
    _updated = ns3::Simulator::Now();
}

TopologyTable::TopologyTable(ns3::Time ttl) :
    _table(),
    _ttl(ttl)
{
}

TopologyTable::iterator TopologyTable::Find(IcaoAddress destination) {
    return iterator(_table.find(destination));
}

void TopologyTable::Insert(Entry entry) {
    _table.insert(std::make_pair(entry.Destination(), entry));
}

void TopologyTable::Remove(iterator position) {
    _table.erase(position.inner());
}

void TopologyTable::RemoveExpired() {
    const auto now = ns3::Simulator::Now();
    for (auto iter = _table.begin(); iter != _table.end(); /* nothing */) {
        const auto age = now - iter->second.Updated();
        if (age > _ttl) {
            const auto to_remove = iter;
            ++iter;
            _table.erase(to_remove);
        } else {
            ++iter;
        }
    }
}

TopologyTable::PrintTable::PrintTable(const TopologyTable& table) :
    _table(table)
{
}
std::ostream& operator << (std::ostream& stream, const TopologyTable::PrintTable& pt) {
    const auto& table = pt._table;
    stream << "| Destination | Last hop | Sequence |\n";
    for (const auto& entry : table) {
        stream << " " << entry.Destination() << " | " << entry.LastHop() << " | " << entry.Sequence() << " |\n";
    }
    return stream;
}

}
