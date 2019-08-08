#pragma once

#include <stddef.h>
#include <string.h>
#include <cstddef>
#include "mem/allocator.hpp"
#include "slice.hpp"

namespace cz {

class String;

struct Str : public Slice<const char> {
    constexpr Str() : Slice({NULL, 0}) {}
    Str(std::nullptr_t) : Slice({NULL, 0}) {}
    Str(const char* cstr) : Slice({cstr, strlen(cstr)}) {}
    constexpr Str(const char* buffer, size_t len) : Slice({buffer, len}) {}

    template <size_t len>
    static constexpr Str cstr(const char (&str)[len]) {
        return {str, len - 1};
    }

    /// Create a new \c String with the same contents in a unique memory buffer.
    String duplicate(mem::Allocator) const;

    bool operator==(const Str& other) const {
        return len == other.len && memcmp(buffer, other.buffer, len) == 0;
    }
    bool operator!=(const Str& other) const { return !(*this == other); }

    bool operator<(const Str& other) const {
        auto x = memcmp(buffer, other.buffer, len < other.len ? len : other.len);
        if (x == 0) {
            return len < other.len;
        }
        return x < 0;
    }
    bool operator>(const Str& other) const { return other < *this; }
    bool operator<=(const Str& other) const { return !(other < *this); }
    bool operator>=(const Str& other) const { return !(*this < other); }
};

}
