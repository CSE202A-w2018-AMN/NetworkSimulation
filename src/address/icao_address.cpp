#include "icao_address.h"
#include <iomanip>

IcaoAddress::IcaoAddress(std::uint32_t address) :
    _address(address & 0xffffff)
{
}

IcaoAddress::IcaoAddress() :
    IcaoAddress(0)
{
}

IcaoAddress IcaoAddress::Broadcast() {
    return IcaoAddress(0xffffff);
}

std::uint32_t IcaoAddress::Value() const {
    return _address;
}

ns3::Address IcaoAddress::ConvertTo() const {
    return ns3::Address(GetType(), reinterpret_cast<const std::uint8_t*>(&_address), 3);
}

IcaoAddress IcaoAddress::ConvertFrom(const ns3::Address& address) {
    std::uint32_t bits = 0;
    NS_ASSERT(address.CheckCompatible(GetType(), 3));
    address.CopyTo(reinterpret_cast<std::uint8_t*>(&bits));
    return IcaoAddress(bits);
}

std::uint8_t IcaoAddress::GetType() {
    static std::uint8_t type = ns3::Address::Register();
    return type;
}

std::ostream& operator << (std::ostream& stream, const IcaoAddress& address) {
    // Save flags for later restoration
    const auto flags(stream.flags());
    stream << "0x" << std::hex << std::nouppercase << std::setw(6) << std::right << std::setfill('0')
        << address._address;
    stream.flags(flags);
    return stream;
}
