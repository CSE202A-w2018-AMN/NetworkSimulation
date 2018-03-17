#ifndef LOG_LOG_H
#define LOG_LOG_H
#include <fstream>
#include <iostream>
#include "address/icao_address.h"

namespace logger {

/**
 * A logger that writes to a file for a specific OLSR instance
 */
class NodeLogger {
private:
    /** The file to write to */
    std::ofstream _file;
    /** The address, used for file opening */
    IcaoAddress _address;
    /** If _address has been initialized */
    bool _address_set;

    /**
     * Pointer to a statically allocated string containing the path to the
     * log directory, without a trailing slash
     */
    static const char* _path;

    void CheckFileOpen();
public:
    NodeLogger();
    /** Writes something to the log, returning *self */
    template <typename T>
    NodeLogger& operator << (const T& value) {
        CheckFileOpen();
        _file << value;
        return *this;
    }

    static inline void InitPath(const char* path) {
        _path = path;
    }
    void Flush();
    void SetAddress(IcaoAddress address);
};

}

#endif
