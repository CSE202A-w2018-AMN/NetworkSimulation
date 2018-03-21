#ifndef PACKET_RECORDER_H
#define PACKET_RECORDER_H
#include <ns3/object.h>
#include <ns3/nstime.h>
#include <map>
#include <cstdint>
#include <string>
#include <ostream>

/**
 * Records information about packets
 */
class PacketRecorder : public ns3::Object {
public:

    enum class PacketType {
        Management,
        Data,
    };

    class Entry {
    public:
        PacketType type;
        ns3::Time time_sent;
        ns3::Time time_received;
        bool received;
        Entry(PacketType type);
    };


    static ns3::TypeId GetTypeId();

    void RecordPacketSent(std::uint64_t id, PacketType type);
    void RecordPacketReceived(std::uint64_t id);

    void WriteCsv(const std::string& path) const;

private:
    /** Packet ID -> entry table */
    std::map<std::uint64_t, Entry> _table;
};

std::ostream& operator << (std::ostream& stream, const PacketRecorder::PacketType& type);

#endif
