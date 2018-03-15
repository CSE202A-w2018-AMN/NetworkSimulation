#include "multipoint_relay.h"
#include "util/print_container.h"
#include <ns3/log.h>
#include <cassert>
#include <vector>
#include <set>
#include <algorithm>
#include <functional>

namespace olsr {

NS_LOG_COMPONENT_DEFINE("olsr::multipoint_relay");

namespace {

bool compare_two_hop_neighbors(const NeighborTableEntry& e1, const NeighborTableEntry& e2) {
    return e1.TwoHopNeighbors().size() > e2.TwoHopNeighbors().size();
}

}

void update_multipoint_relay(NeighborTable* table) {
    NS_LOG_FUNCTION(table);
    assert(table);
    // General idea:
    // Choose a subset of bidirectional neighbors that provide access
    // to all 2-hop neighbors

    // Heuristic: Sort from most to least 2-hop neighbors, add to the subset
    // until no more 2-hop neighbors get added

    // Part 1: Collect bidirectional or MPR neighbors
    std::vector<std::reference_wrapper<const NeighborTableEntry>> bidir_neighbors;
    bidir_neighbors.reserve(table->size());
    for (const auto& entry : *table) {
        const auto& table_entry = entry.second;
        const auto state = table_entry.State();
        if (state == LinkState::Bidirectional || state == LinkState::MultiPointRelay) {
            bidir_neighbors.push_back(table_entry);
        }
    }
    // Part 2: Sort by number of 2-hop neighbors, greatest to least
    std::sort(bidir_neighbors.begin(), bidir_neighbors.end(), compare_two_hop_neighbors);
    NS_LOG_LOGIC("Bidirectional neighbors sorted: " << print_container::print(bidir_neighbors));

    std::set<IcaoAddress> two_hop_neighbors;
    std::set<IcaoAddress> multipoint_relay;
    for (const NeighborTableEntry& neighbor : bidir_neighbors) {
        const auto initial_size = two_hop_neighbors.size();
        two_hop_neighbors.insert(neighbor.TwoHopNeighbors().begin(), neighbor.TwoHopNeighbors().end());
        const auto new_size = two_hop_neighbors.size();
        // Make this neighbor a multipoint relay if it added a new 2-hop neighbor
        if (initial_size != new_size) {
            multipoint_relay.insert(neighbor.Address());
        }
    }
    // Update table entries: If bidirectional or multipoint relay,
    // set to bidirectional or multipoint based on multipoint_relay presence
    for (auto& entry : *table) {
        auto& table_entry = entry.second;
        const auto state = table_entry.State();
        if (state == LinkState::Bidirectional || state == LinkState::MultiPointRelay) {
            if (multipoint_relay.find(table_entry.Address()) != multipoint_relay.end()) {
                table_entry.SetState(LinkState::MultiPointRelay);
            } else {
                table_entry.SetState(LinkState::Bidirectional);
            }
        }
    }
}

}
