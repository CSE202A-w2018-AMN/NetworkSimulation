#ifndef RECORDER_RECORD_H
#define RECORDER_RECORD_H
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <map>
#include "address/icao_address.h"
#include "network/olsr/routing_table.h"

namespace record {
using boost::posix_time::ptime;

/**
 * The status of a node at some point in time
 */
class NodeRecord {
public:
    double latitude;
    double longitude;
    double altitude;
    olsr::RoutingTable routing;
    /** Prints this node record to a stream as JSON */
    void PrintJson(std::ostream& stream) const;
};

/**
 * A recorded time point
 */
class Record {
private:
    /** The time */
    ptime _time;
    /** Address -> node record map */
    std::map<IcaoAddress, NodeRecord> _nodes;
public:
    Record(ptime time);
    void AddNode(IcaoAddress address, NodeRecord&& record);
    /** Prints this record to a stream as JSON */
    void PrintJson(std::ostream& stream) const;
};

}

#endif
