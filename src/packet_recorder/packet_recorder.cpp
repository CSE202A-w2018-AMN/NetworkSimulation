#include "packet_recorder.h"
#include <ns3/simulator.h>
#include <ns3/log.h>
#include <fstream>

NS_LOG_COMPONENT_DEFINE("PacketRecorder");

namespace {

void PrintPreciseSeconds(std::ostream& stream, const ns3::Time& time) {
    const auto flags(stream.flags());
    const auto seconds = time.GetSeconds();
    stream.precision(16);
    stream << seconds;
    stream.flags(flags);
}

}

PacketRecorder::Entry::Entry(PacketType type) :
    type(type),
    time_sent(ns3::Simulator::Now()),
    time_received(),
    received(false)
{
}

void PacketRecorder::RecordPacketSent(std::uint64_t id, PacketType type) {
    _table.insert(std::make_pair(id, Entry(type)));
}

void PacketRecorder::RecordPacketReceived(std::uint64_t id) {
    auto in_table = _table.find(id);
    if (in_table != _table.end()) {
        in_table->second.received = true;
        in_table->second.time_received = ns3::Simulator::Now();
    } else {
        NS_LOG_WARN("No entry found for packet " << id);
    }
}

void PacketRecorder::WriteCsv(const std::string& path) const {
    std::ofstream file;
    file.exceptions(std::ios::badbit | std::ios::failbit);
    file.open(path, std::ios::out | std::ios::trunc);
    file << "ID,Type,SentSec,ReceivedSec\n";
    for (const auto& pair : _table) {
        const auto id = pair.first;
        const auto& entry = pair.second;
        file << id << ',' << entry.type << ',';
        PrintPreciseSeconds(file, entry.time_sent);
        file << ',';
        if (entry.received) {
            PrintPreciseSeconds(file, entry.time_received);
        }
        file << '\n';
    }
}

std::ostream& operator << (std::ostream& stream, const PacketRecorder::PacketType& type) {
    switch (type) {
    case PacketRecorder::PacketType::Management:
        stream << "Management";
        break;
    case PacketRecorder::PacketType::Data:
        stream << "Data";
        break;
    }
    return stream;
}

ns3::TypeId PacketRecorder::GetTypeId() {
    static ns3::TypeId id = ns3::TypeId("PacketRecorder")
        .SetParent<ns3::Object>()
        .AddConstructor<PacketRecorder>();
    return id;
}
