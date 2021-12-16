module;

#include <intrin.h>

export module mask;

import basictypes; // Word, Dword, Qword

export using Bits = unsigned char;
export using SBits = char;

export template <SBits offset, typename Flags>
void BitSet(Flags& flags, bool value)
{
    constexpr Bits offset_ = offset >= 0 ? offset : 8 * sizeof(Flags) + offset;
    constexpr Flags one = 1;

    flags = flags & ~(one << offset_) | (value << offset_);
}

export template <SBits offset>
bool BitTest(const Dword& flags)
{
    constexpr Bits offset_ = offset >= 0 ? offset : 8 * sizeof(Dword) + offset;

    return _bittest((const long*)&flags, offset_);
}

export template <SBits offset>
bool BitTest(const Qword& flags)
{
    constexpr Bits offset_ = offset >= 0 ? offset : 8 * sizeof(Qword) + offset;

    return _bittest64((const long long*)&flags, offset_);
}

// Generate a bitmask of ones
template <typename Type>
consteval Type FillMask(Bits nbits)
{
    Type mask = 0;

    for (unsigned char index = 0; index < nbits; ++index) {
        mask = (mask << 1) | 1;
    }

    return mask;
}

// Generate a mask for the destination variable
template <typename Destination>
consteval Destination DestinationMask(Bits nbits, Bits offset)
{
    return FillMask<Destination>(nbits) << offset;
}

template <typename Destination>
consteval Destination NotDestinationMask(Bits nbits, Bits offset)
{
    // Just to make sure, even though it's not necessary, we do "& FillMask<Destination>(8 * sizeof(Destination))"
    return ~DestinationMask<Destination>(nbits, offset) & FillMask<Destination>(8 * sizeof(Destination));
}

template <typename Destination>
consteval Destination SourceMask(Bits nbits)
{
    return FillMask<Destination>(nbits);
}

export template <typename Source, Bits nbits, SBits offset, typename Destination>
Source GetWithMask(const Destination& destination)
{
    constexpr Bits offset_ = offset >= 0 ? offset : 8 * sizeof(Destination) + offset;

    return (Source)((destination & DestinationMask<Destination>(nbits, offset_)) >> offset_);
}

export template <Bits nbits, SBits offset, typename Destination, typename Source>
void SetWithMask(Destination& destination, const Source& source)
{
    constexpr Bits offset_ = offset >= 0 ? offset : 8 * sizeof(Destination) + offset;

    destination = (source & SourceMask<Destination>(nbits)) << offset_ | (destination & NotDestinationMask<Destination>(nbits, offset_));
}

export template <Bits nbits, SBits offset, typename Destination, typename Enum>
void SetEnumWithMask(Destination& destination, const Enum& source)
{
    SetWithMask<nbits, offset, Destination, __underlying_type(Enum)>(destination, (__underlying_type(Enum)&)source);
}
