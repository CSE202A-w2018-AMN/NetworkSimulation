#include "routing_calc.h"
#include <cassert>
#include <limits>

namespace olsr {

void calculate_routes(RoutingTable* routing, const NeighborTable& neighbors, const TopologyTable& topology) {
    assert(routing);
    // Part 1: Reset
    routing->clear();
    // Part 2: Neighbors
    for (const auto& entry : neighbors) {
        const auto& neighbor_entry = entry.second;
        const auto address = neighbor_entry.Address();
        const auto state = neighbor_entry.State();
        if (state == LinkState::Bidirectional || state == LinkState::MultiPointRelay) {
            // Add a 1-hop route to this neighbor
            RoutingTable::Entry entry(address, address, 1);
            routing->Insert(entry);
        }
    }
    // Part 3: Non-neighbors
    for (std::uint16_t h = 1; h != std::numeric_limits<std::uint16_t>::max() - 1; ++h) {
        auto entry_added = false;

        // Look in the topology table
        for (const auto& topology_entry : topology) {
            const auto dest_in_routing = routing->Find(topology_entry.Destination());
            if (dest_in_routing == routing->end()) {
                // Not in the routing table, look for a route to the last hop
                const auto last_hop_in_routing = routing->Find(topology_entry.LastHop());
                if (last_hop_in_routing != routing->end()) {
                    if (last_hop_in_routing->Distance() == h) {
                        // This is a new segment
                        RoutingTable::Entry entry{
                            topology_entry.Destination(),
                            last_hop_in_routing->NextHop(),
                            static_cast<std::uint16_t>(h + 1u)};
                        routing->Insert(entry);
                        entry_added = true;
                    }
                }
            }
        }

        if (!entry_added) {
            break;
        }
    }
}

}
