module;

#include "error.h"

export module pattern;

import basictypes; // Byte

export template<size_t length>
class Pattern {
public:
    constexpr Pattern(const char(&pattern)[length], int padding = 0x0) : padding(padding), pattern(pattern)
    {
        for (size_t index = 0; index < length / 3; ++index) {
            char first = pattern[3 * index], second = pattern[3 * index + 1];

            if (first == '?' || second == '?') {
                bytes[index] = 0x00;
                mask[index] = '?';
            }
            else {
                bytes[index] = 0x01 * ToByte(first) + 0x10 * ToByte(second);
                mask[index] = 'x';
            }
        }
    }

    Byte* Find(Byte* start, size_t size, unsigned int skip = 0)
    {
        Byte* const end = start + size - sizeof(mask) + 1;

        for (Byte* pointer = start; pointer < end; ++pointer) {
            bool found = true;

            for (size_t index = 0; index < sizeof(mask); ++index)
            {
                found = mask[index] == '?' || *(pointer + index) == bytes[index];

                if (!found)
                {
                    break;
                }
            }

            if (found) {
                if (skip == 0)
                {
                    return pointer + padding;
                }

                --skip;
            }
        }

        return nullptr;
    }

public:
    Byte bytes[length / 3];
    char mask[length / 3];
    int padding;
    const char(&pattern)[length];

    static constexpr Byte ToByte(char character)
    {
        if (character >= '0' && character <= '9') {
            return character - '0';
        }
        else if (character >= 'A' && character <= 'F') {
            return character - 'A' + 10;
        }
        else if (character >= 'a' && character <= 'f') {
            return character - 'a' + 10;
        }

        throw MinorError("Invalid byte pattern.");
    }
};
