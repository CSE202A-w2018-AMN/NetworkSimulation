#ifndef RECORDER_SESSION_RECORDER_H
#define RECORDER_SESSION_RECORDER_H
#include "session.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <ns3/nstime.h>
#include <ns3/node-container.h>

namespace record {
using boost::posix_time::ptime;

/** Records a session from an NS3 simulation */
class SessionRecorder {
private:
    /** The real-world time when the simulation started */
    ptime _simulation_start;
    /** Time interval between records */
    ns3::Time _interval;
    /** Nodes to record */
    ns3::NodeContainer _nodes;
    /** The session being constructed */
    Session _session;

    /** Records a record */
    void RecordRecord();
    /** Returns the current time in the simulation converted into a ptime */
    ptime NowRealTime();
public:
    /** Creates a recorder */
    SessionRecorder(ptime simulation_start, ns3::Time interval, ns3::NodeContainer&& nodes);
    /** Schedules recording */
    void Start();

    /** Returns the session */
    inline const Session& GetSession() const {
        return _session;
    }

    /** Writes the session to a JSON file at the specified path */
    void WriteJson(const std::string& path);
};

}

#endif
