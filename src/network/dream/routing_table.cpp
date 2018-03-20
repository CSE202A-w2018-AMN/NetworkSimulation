#include "routing_table.h"

namespace dream {

RoutingTable::Entry::Entry(IcaoAddress destination, IcaoAddress next_hop, std::uint16_t distance) :
    _destination(destination),
    _next_hop(next_hop),
    _distance(distance)
{
}

RoutingTable::iterator RoutingTable::Find(IcaoAddress destination) {
    return iterator(_table.find(destination));
}
void RoutingTable::Insert(Entry entry) {
    _table.insert(std::make_pair(entry.Destination(), entry));
}



RoutingTable::PrintTable::PrintTable(const RoutingTable& table) :
    _table(table)
{
}

std::ostream& operator << (std::ostream& stream, const RoutingTable::PrintTable& pt) {
    stream << "| Destination | Next hop | Distance |\n";
    for (const auto& entry : pt._table) {
        stream << "|    " << entry.Destination() << " | "
            << entry.NextHop() << " |  " << entry.Distance() << " |\n";
    }
    return stream;
}

}
