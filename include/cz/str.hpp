#pragma once

#include <stddef.h>
#include <string.h>
#include <cstddef>
#include "allocator.hpp"
#include "assert.hpp"
#include "char_type.hpp"
#include "slice.hpp"
#include "vector.hpp"

namespace cz {

struct String;

struct Str {
    const char* buffer;
    size_t len;

    Str() = default;
    constexpr Str(std::nullptr_t) : buffer(nullptr), len(0) {}
    Str(const char* cstr) : buffer(cstr), len(strlen(cstr)) {}
    constexpr Str(const char* buffer, size_t len) : buffer(buffer), len(len) {}

    template <size_t len>
    static constexpr Str cstr(const char (&str)[len]) {
        return {str, len - 1};
    }

    /// Create a new \c String with the same contents in a unique memory buffer.
    String clone(Allocator) const;
    /// Create a new \c String with the same contents in a unique memory buffer
    /// and allocate space for a null terminator.
    String clone_null_terminate(Allocator) const;

    constexpr const char* start() const { return buffer; }
    constexpr const char* end() const { return buffer + len; }

    char first() const {
        CZ_DEBUG_ASSERT(len > 0);
        return buffer[0];
    }
    char last() const {
        CZ_DEBUG_ASSERT(len > 0);
        return buffer[len - 1];
    }

    bool equals_case_insensitive(Str other) const;

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

    bool starts_with_case_insensitive(Str prefix) const;
    bool ends_with_case_insensitive(Str postfix) const;

    bool starts_with(char c) const { return len > 0 && buffer[0] == c; }
    bool ends_with(char c) const { return len > 0 && buffer[len - 1] == c; }
    bool starts_with_case_insensitive(char c) const {
        return len > 0 && cz::to_lower(buffer[0]) == cz::to_lower(c);
    }
    bool ends_with_case_insensitive(char c) const {
        return len > 0 && cz::to_lower(buffer[len - 1]) == cz::to_lower(c);
    }

    size_t count(char c) const {
        cz::Str cpy = *this;
        size_t cnt = 0;
        while (1) {
            const char* spot = cpy.find(c);
            if (spot) {
                ++cnt;
                cpy.buffer = spot + 1;
                cpy.len = len + buffer - cpy.buffer;
            } else {
                return cnt;
            }
        }
    }

    bool contains(Str infix) const { return find(infix); }
    bool contains(char infix) const { return find(infix); }
    bool contains_case_insensitive(Str infix) const { return find_case_insensitive(infix); }
    bool contains_case_insensitive(char infix) const { return find_case_insensitive(infix); }

    const char* find(Str infix) const;
    const char* rfind(Str infix) const;

    const char* find(char pattern) const { return (const char*)memchr(buffer, pattern, len); }
    const char* rfind(char pattern) const;

    const char* find_case_insensitive(Str infix) const;
    const char* rfind_case_insensitive(Str infix) const;

    const char* find_case_insensitive(char pattern) const {
        return find_case_insensitive({&pattern, 1});
    }
    const char* rfind_case_insensitive(char pattern) const {
        return rfind_case_insensitive({&pattern, 1});
    }

    /// Index overloads return `len` on no match.
    size_t find_index(Str infix) const {
        const char* ptr = find(infix);
        if (!ptr)
            return len;
        return ptr - buffer;
    }
    size_t rfind_index(Str infix) const {
        const char* ptr = rfind(infix);
        if (!ptr)
            return len;
        return ptr - buffer;
    }
    size_t find_index(char pattern) const {
        const char* ptr = find(pattern);
        if (!ptr)
            return len;
        return ptr - buffer;
    }
    size_t rfind_index(char pattern) const {
        const char* ptr = rfind(pattern);
        if (!ptr)
            return len;
        return ptr - buffer;
    }
    size_t find_index_case_insensitive(Str infix) const {
        const char* ptr = find_case_insensitive(infix);
        if (!ptr)
            return len;
        return ptr - buffer;
    }
    size_t rfind_index_case_insensitive(Str infix) const {
        const char* ptr = rfind_case_insensitive(infix);
        if (!ptr)
            return len;
        return ptr - buffer;
    }
    size_t find_index_case_insensitive(char pattern) const {
        const char* ptr = find_case_insensitive(pattern);
        if (!ptr)
            return len;
        return ptr - buffer;
    }
    size_t rfind_index_case_insensitive(char pattern) const {
        const char* ptr = rfind_case_insensitive(pattern);
        if (!ptr)
            return len;
        return ptr - buffer;
    }

    cz::Str slice(size_t start, size_t end) const { return {buffer + start, end - start}; }
    cz::Str slice(const char* start, size_t end) const { return slice(start - buffer, end); }
    cz::Str slice(size_t start, const char* end) const { return slice(start, end - buffer); }
    cz::Str slice(const char* start, const char* end) const {
        return slice(start - buffer, end - buffer);
    }

    cz::Str slice_start(size_t start) const { return slice(start, len); }
    cz::Str slice_start(const char* start) const { return slice(start, len); }

    cz::Str slice_end(size_t end) const { return slice((size_t)0, end); }
    cz::Str slice_end(const char* end) const { return slice((size_t)0, end); }

    void split_into(char separator, cz::Allocator allocator, cz::Vector<cz::Str>* values);

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
