#pragma once

#include <stdint.h>

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

}
