#include "record.h"
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace record {

namespace {

void PrintRoutingEntry(std::ostream& stream, const olsr::RoutingTable::Entry& entry) {
    stream << "{\"destination\":" << entry.Destination().Value()
        << ",\"next_hop\":" << entry.NextHop().Value()
        << ",\"distance\":" << entry.Distance() << '}';
}

std::string format_time_iso(ptime time) {
    return boost::posix_time::to_iso_extended_string(time);
}

}

void NodeRecord::PrintJson(std::ostream& stream) const {
    stream << "{\"latitude\":" << latitude
        << ",\"longitude\":" << longitude
        << ",\"altitude\":" << altitude;
    stream << ",\"routes\":[";
    bool first = true;
    for (const auto& route_entry : routing) {
        if (!first) {
            stream << ',';
        }
        PrintRoutingEntry(stream, route_entry);
        first = false;
    }
    stream << "]}";
}

Record::Record(ptime time) :
    _time(time)
{
}

void Record::AddNode(IcaoAddress address, NodeRecord&& record) {
    _nodes.insert(std::make_pair(address, record));
}

void Record::PrintJson(std::ostream& stream) const {
    stream << "{\"time\":\"" << format_time_iso(_time) << "\",\"nodes\":{";
    auto first = true;
    for (const auto& entry : _nodes) {
        if (!first) {
            stream << ',';
        }
        stream << '"' << entry.first.Value() << "\":";
        entry.second.PrintJson(stream);
        first = false;
    }
    stream << "}}";
}

}
