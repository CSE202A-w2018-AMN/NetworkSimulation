#ifndef RECORDER_SESSION_H
#define RECORDER_SESSION_H
#include "record.h"
#include <vector>
#include <ostream>

namespace record {

/**
 * A session containing zero or more Records
 */
class Session {
private:
    /** Records, sorted by time */
    std::vector<Record> _records;
public:
    /** Appends a record */
    void AddRecord(Record&& record);
    /** Prints this record to a stream as JSON */
    void PrintJson(std::ostream& stream) const;
};


}

#endif
