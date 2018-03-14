#ifndef NETWORK_OLSR_MULTIPOINT_RELAY_H
#define NETWORK_OLSR_MULTIPOINT_RELAY_H
#include "neighbor_table.h"

namespace olsr {

/**
 * Updates the multipoint relay set in a neighbor table
 *
 * @param table a non-NULL pointer to a neighbor table
 */
void update_multipoint_relay(NeighborTable* table);

}

#endif
