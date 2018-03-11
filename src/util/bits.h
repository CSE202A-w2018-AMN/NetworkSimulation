#ifndef UTIL_BITS_H
#define UTIL_BITS_H
#include <cstdint>
#include <ns3/buffer.h>

namespace bits {

/**
 * Reads a 24-bit integer from the provided iterator in network byte order
 * and returns it
 */
std::uint32_t read_u24(ns3::Buffer::Iterator* source);

/**
 * Writes a 24-bit integer to the provided iterator in network byte order
 */
void write_u24(ns3::Buffer::Iterator* dest, std::uint32_t bits);

}

#endif
