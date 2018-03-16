#ifndef NETWORK_OLSR_ROUTING_CALC_H
#define NETWORK_OLSR_ROUTING_CALC_H

#include "routing_table.h"
#include "neighbor_table.h"
#include "topology_table.h"

namespace olsr {

/**
 * Updates a routing table based on the neighbors and topology
 * @param routing a non-NULL pointer to a RoutingTable
 */
void calculate_routes(RoutingTable* routing, const NeighborTable& neighbors, const TopologyTable& topology);

}

#endif
