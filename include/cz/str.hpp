#pragma once

#include <stddef.h>
#include <string.h>
#include <cstddef>
#include "assert.hpp"
#include "mem/allocator.hpp"
#include "slice.hpp"

namespace cz {

class String;

struct Str {
    const char* buffer;
    size_t len;

    constexpr Str() : buffer(nullptr), len(0) {}
    constexpr Str(std::nullptr_t) : Str() {}
    Str(const char* cstr) : buffer(cstr), len(strlen(cstr)) {}
    constexpr Str(const char* buffer, size_t len) : buffer(buffer), len(len) {}

    template <size_t len>
    static constexpr Str cstr(const char (&str)[len]) {
        return {str, len - 1};
    }

    /// Create a new \c String with the same contents in a unique memory buffer.
    String duplicate(mem::Allocator) const;

    constexpr const char* start() const { return buffer; }
    constexpr const char* end() const { return buffer + len; }

    bool starts_with(Str prefix) const {
        if (len < prefix.len) {
            return false;
        } else {
            return memcmp(buffer, prefix.buffer, prefix.len) == 0;
        }
    }

    bool ends_with(Str postfix) const {
        if (len < postfix.len) {
            return false;
        } else {
            return memcmp(buffer + (len - postfix.len), postfix.buffer, postfix.len) == 0;
        }
    }

    const char* find(char pattern) const {
        return static_cast<const char*>(memchr(buffer, pattern, len));
    }
    const char* rfind(char pattern) const;

    char operator[](size_t index) const {
        CZ_DEBUG_ASSERT(index < len);
        return buffer[index];
    }

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
