#include "routing_calc.h"
#include <cassert>

namespace olsr {

void calculate_routes(RoutingTable* routing, const NeighborTable& neighbors, const TopologyTable& topology) {
    assert(routing);
    // Part 1: Reset
    routing->clear();
    // Part 2: Neighbors
    
}

}
