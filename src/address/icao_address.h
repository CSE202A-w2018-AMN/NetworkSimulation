#ifndef ICAO_ADDRESS_H
#define ICAO_ADDRESS_H
#include <cstdint>
#include <ostream>
#include <ns3/address.h>

/**
 * A 24-bit ICAO address
 */
class IcaoAddress {
public:
    /**
     * Creates an address
     * @param address the 24-bit address. The 8 most significant bits will
     * be ignored.
     */
    explicit IcaoAddress(std::uint32_t address);

    /**
     * Returns the broadcast address, 0xffffff
     */
    static IcaoAddress Broadcast();

    /**
     * Creates a zero address
     */
    IcaoAddress();

    /**
     * @return the bits of this address, with the 8 most significant bits
     * set to zero
     */
    std::uint32_t Value() const;

    /**
     * Converts this address into a generic NS3 address
     */
    ns3::Address ConvertTo() const;

    /**
     * Creates an ICAO address from a generic NS3 address
     */
    static IcaoAddress ConvertFrom(const ns3::Address& address);

    /**
     * Writes an address to a stream
     */
    friend  std::ostream& operator << (std::ostream& stream, const IcaoAddress& address);

    /** Compares addresses */
    friend inline bool operator == (const IcaoAddress& a1, const IcaoAddress& a2) {
        return a1._address == a2._address;
    }
    friend inline bool operator != (const IcaoAddress& a1, const IcaoAddress& a2) {
        return a1._address != a2._address;
    }
    /** Compares addresses */
    friend inline bool operator < (const IcaoAddress& lhs, const IcaoAddress& rhs) {
        return lhs._address < rhs._address;
    }
    friend inline bool operator <= (const IcaoAddress& lhs, const IcaoAddress& rhs) {
        return lhs._address <= rhs._address;
    }
    friend inline bool operator > (const IcaoAddress& lhs, const IcaoAddress& rhs) {
        return lhs._address > rhs._address;
    }
    friend inline bool operator >= (const IcaoAddress& lhs, const IcaoAddress& rhs) {
        return lhs._address >= rhs._address;
    }

private:
    /**
     * The address, stored in the lower 24 bits
     * The high 8 bits are always zero.
     */
    std::uint32_t _address;

    /** Returns an address type value for this type */
    static std::uint8_t GetType();
};

#endif
