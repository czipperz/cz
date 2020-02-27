#pragma once

#include <stdint.h>
#include <cz/str.hpp>

namespace cz {

using Hash = uint64_t;

inline Hash hash(cz::Str str, Hash base) {
    for (size_t i = 0; i < str.len; ++i) {
        base *= 31;
        base += str[i];
    }
    return base;
}

}
