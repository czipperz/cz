#pragma once

#include <stdint.h>
#include <stddef.h>

namespace cz {

inline uint8_t next_power_of_two(uint8_t value) {
    value |= (value >> 1);
    value |= (value >> 2);
    value |= (value >> 4);
    return value + 1;
}

inline uint16_t next_power_of_two(uint16_t value) {
    value |= (value >> 1);
    value |= (value >> 2);
    value |= (value >> 4);
    value |= (value >> 8);
    return value + 1;
}

inline uint32_t next_power_of_two(uint32_t value) {
    value |= (value >> 1);
    value |= (value >> 2);
    value |= (value >> 4);
    value |= (value >> 8);
    value |= (value >> 16);
    return value + 1;
}

inline uint64_t next_power_of_two(uint64_t value) {
    value |= (value >> 1);
    value |= (value >> 2);
    value |= (value >> 4);
    value |= (value >> 8);
    value |= (value >> 16);
    value |= (value >> 32);
    return value + 1;
}

// On Apple Clang, size_t is 'unsigned long' which is the same width but a
// distinct type from 'unsigned long long' which is how 'uint64_t' is defined.
#if __APPLE__ && __SIZEOF_SIZE_T__ == 8
inline uint64_t next_power_of_two(size_t value) {
    return next_power_of_two((uint64_t)value);
}
#endif

}
