#include "routing_table.h"

namespace olsr {

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


}
