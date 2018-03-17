#include "log.h"
#include <cassert>
#include <sstream>
#include <iostream>

namespace logger {

const char* NodeLogger::_path = nullptr;

NodeLogger::NodeLogger() :
    _address_set(false)
{
    _file.exceptions(std::ios::badbit | std::ios::failbit);
}

void NodeLogger::Flush() {
    if (_file.is_open()) {
        _file << std::flush;
    }
}

void NodeLogger::SetAddress(IcaoAddress address) {
    _address = address;
    _address_set = true;
}

void NodeLogger::CheckFileOpen() {
    if (!_file.is_open()) {
        assert(_address_set);
        assert(_path);
        std::stringstream stream;
        stream << _path << '/' << _address << ".txt";
        std::cerr << "Opening " << stream.str() << '\n';
        _file.open(stream.str(), std::ios::out | std::ios::trunc);
    }
}

}
