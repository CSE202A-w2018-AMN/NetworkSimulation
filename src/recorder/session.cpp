#include "session.h"

namespace record {

void Session::AddRecord(Record&& record) {
    _records.push_back(record);
}

void Session::PrintJson(std::ostream& stream) const {
    auto first = true;
    stream << '[';
    for (const auto& record : _records) {
        if (!first) {
            stream << ',';
        }
        record.PrintJson(stream);
        first = false;
    }
    stream << ']';
}

}
