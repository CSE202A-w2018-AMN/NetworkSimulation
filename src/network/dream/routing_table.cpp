#include "routing_table.h"
#include <ns3/simulator.h>

namespace dream {

RoutingTable::Entry::Entry(IcaoAddress destination, const ns3::Vector& location, const ns3::Vector& velocity) :
    _destination(destination),
    _location(location),
    _velocity(velocity),
    _last_time(ns3::Simulator::Now())
{
}

void RoutingTable::Entry::MarkSeen() {
    _last_time = ns3::Simulator::Now();
}

RoutingTable::RoutingTable(const ns3::Time& ttl) :
    _table(),
    _ttl(ttl)
{
}

RoutingTable::iterator RoutingTable::Find(IcaoAddress destination) {
    return iterator(_table.find(destination));
}
void RoutingTable::Insert(Entry entry) {
    _table.insert(std::make_pair(entry.Destination(), entry));
}

void RoutingTable::RemoveExpired() {
    const auto now = ns3::Simulator::Now();
    for (auto iter = _table.begin(); iter != _table.end(); /* nothing */) {
        const auto age = now - iter->second.LastTime();
        if (age > _ttl) {
            const auto to_remove = iter;
            ++iter;
            _table.erase(to_remove);
        } else {
            ++iter;
        }
    }
}

RoutingTable::PrintTable::PrintTable(const RoutingTable& table) :
    _table(table)
{
}

std::ostream& operator << (std::ostream& stream, const RoutingTable::PrintTable& pt) {
    stream << "| Destination | Position | Velocity |\n";
    for (const auto& entry : pt._table) {
        stream << "|    " << entry.Destination() << " | "
            << entry.Location() << " |  " << entry.Velocity() << " |\n";
    }
    return stream;
}

}
