#include "bits.h"

namespace bits {

std::uint32_t read_u24(ns3::Buffer::Iterator* source) {
    std::uint32_t bits = 0;
    bits |= static_cast<std::uint32_t>(source->ReadU8()) << 16;
    bits |= static_cast<std::uint32_t>(source->ReadU8()) << 8;
    bits |= static_cast<std::uint32_t>(source->ReadU8());
    return bits;
}

void write_u24(ns3::Buffer::Iterator* dest, std::uint32_t bits) {
    dest->WriteU8(static_cast<std::uint8_t>(bits >> 16));
    dest->WriteU8(static_cast<std::uint8_t>(bits >> 8));
    dest->WriteU8(static_cast<std::uint8_t>(bits));
}

}
