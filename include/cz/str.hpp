#pragma once

#include <stddef.h>
#include <string.h>
#include <cstddef>
#include "allocator.hpp"
#include "assert.hpp"
#include "char_type.hpp"
#include "ptr.hpp"
#include "slice.hpp"
#include "vector.hpp"

namespace cz {

struct String;

/// An immutable slice of a string.
struct Str {
    const char* buffer;
    size_t len;

    /// Convenience constructors.
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

    /// Must manually bounds check!
    char operator[](size_t index) const { return get(index); }
    char get(size_t index) const {
        CZ_DEBUG_ASSERT(index < len);
        return buffer[index];
    }

    /// Simple comparison operators.
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

    bool equals_case_insensitive(Str other) const;

    /// Combined slice and comparison methods.
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

    /// Count the number of occurrences of a character in the string.
    size_t count(char c) const {
        size_t start = 0;
        size_t cnt = 0;
        while (1) {
            const char* spot = slice_start(start).find(c);
            if (!spot) {
                break;
            }

            ++cnt;
            start = spot + 1 - buffer;
        }
        return cnt;
    }

    /// Check if the string contains another string or a character.
    bool contains(Str infix) const { return find(infix); }
    bool contains(char infix) const { return find(infix); }
    bool contains_case_insensitive(Str infix) const { return find_case_insensitive(infix); }
    bool contains_case_insensitive(char infix) const { return find_case_insensitive(infix); }

    /// Find the first / last instance of the `infix`/`pattern`.
    /// Returns `nullptr` on no match.
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

    /// Or overloads return `otherwise` on no match.
    size_t find_or(Str infix, size_t otherwise) const {
        return orelse(find(infix), buffer, otherwise);
    }
    size_t rfind_or(Str infix, size_t otherwise) const {
        return orelse(rfind(infix), buffer, otherwise);
    }
    size_t find_or(char pattern, size_t otherwise) const {
        return orelse(find(pattern), buffer, otherwise);
    }
    size_t rfind_or(char pattern, size_t otherwise) const {
        return orelse(rfind(pattern), buffer, otherwise);
    }
    size_t find_or_case_insensitive(Str infix, size_t otherwise) const {
        return orelse(find_case_insensitive(infix), buffer, otherwise);
    }
    size_t rfind_or_case_insensitive(Str infix, size_t otherwise) const {
        return orelse(rfind_case_insensitive(infix), buffer, otherwise);
    }
    size_t find_or_case_insensitive(char pattern, size_t otherwise) const {
        return orelse(find_case_insensitive(pattern), buffer, otherwise);
    }
    size_t rfind_or_case_insensitive(char pattern, size_t otherwise) const {
        return orelse(rfind_case_insensitive(pattern), buffer, otherwise);
    }

    /// Index overloads return `len` on no match.
    size_t find_index(Str infix) const { return find_or(infix, len); }
    size_t rfind_index(Str infix) const { return rfind_or(infix, len); }
    size_t find_index(char pattern) const { return find_or(pattern, len); }
    size_t rfind_index(char pattern) const { return rfind_or(pattern, len); }
    size_t find_index_case_insensitive(Str infix) const {
        return find_or_case_insensitive(infix, len);
    }
    size_t rfind_index_case_insensitive(Str infix) const {
        return rfind_or_case_insensitive(infix, len);
    }
    size_t find_index_case_insensitive(char pattern) const {
        return find_or_case_insensitive(pattern, len);
    }
    size_t rfind_index_case_insensitive(char pattern) const {
        return rfind_or_case_insensitive(pattern, len);
    }

    /// Split the string into two pieces divided by the first occurrence of the separator.
    ///
    /// If the separator is found, returns `true` and fills `*before` and `*after`.
    /// Otherwise it returns `false` and doesn't modify the pointers.
    ///
    /// You can safely pass `this` as either `before` or `after`.
    bool split_excluding(char separator, cz::Str* before, cz::Str* after) const;
    bool split_excluding(cz::Str separator, cz::Str* before, cz::Str* after) const;
    bool split_before(char separator, cz::Str* before, cz::Str* after) const;
    bool split_before(cz::Str separator, cz::Str* before, cz::Str* after) const;
    bool split_after(char separator, cz::Str* before, cz::Str* after) const;
    bool split_after(cz::Str separator, cz::Str* before, cz::Str* after) const;

    /// Same as above except divides by the last occurrence of the separator.
    bool split_excluding_last(char separator, cz::Str* before, cz::Str* after) const;
    bool split_excluding_last(cz::Str separator, cz::Str* before, cz::Str* after) const;
    bool split_before_last(char separator, cz::Str* before, cz::Str* after) const;
    bool split_before_last(cz::Str separator, cz::Str* before, cz::Str* after) const;
    bool split_after_last(char separator, cz::Str* before, cz::Str* after) const;
    bool split_after_last(cz::Str separator, cz::Str* before, cz::Str* after) const;

    /// Split the string into pieces excluding the separator.
    void split_into(char separator, cz::Allocator allocator, cz::Vector<cz::Str>* values) const;
    void split_into(cz::Str separator, cz::Allocator allocator, cz::Vector<cz::Str>* values) const;
    /// Same as `split_into` except clones each piece using `string_allocator`.
    void split_clone(char separator,
                     cz::Allocator vector_allocator,
                     cz::Allocator string_allocator,
                     cz::Vector<cz::Str>* values) const;
    void split_clone(cz::Str separator,
                     cz::Allocator vector_allocator,
                     cz::Allocator string_allocator,
                     cz::Vector<cz::Str>* values) const;
    /// Same as `split_into` except clones each piece using `string_allocator` and null terminates.
    void split_clone_nt(char separator,
                        cz::Allocator vector_allocator,
                        cz::Allocator string_allocator,
                        cz::Vector<cz::Str>* values) const;
    void split_clone_nt(cz::Str separator,
                        cz::Allocator vector_allocator,
                        cz::Allocator string_allocator,
                        cz::Vector<cz::Str>* values) const;
    /// Split string into lines, discarding a trailing newline.
    void lines(cz::Allocator allocator, cz::Vector<cz::Str>* values) const;

    /// Take a substring of the string.
    cz::Str slice(size_t start, size_t end) const {
        CZ_DEBUG_ASSERT(start <= len);
        CZ_DEBUG_ASSERT(end <= len);
        return {buffer + start, end - start};
    }
    cz::Str slice(const char* start, size_t end) const { return slice(start - buffer, end); }
    cz::Str slice(size_t start, const char* end) const { return slice(start, end - buffer); }
    cz::Str slice(const char* start, const char* end) const {
        return slice(start - buffer, end - buffer);
    }

    cz::Str slice_start(size_t start) const { return slice(start, len); }
    cz::Str slice_start(const char* start) const { return slice(start, len); }

    cz::Str slice_end(size_t end) const { return slice((size_t)0, end); }
    cz::Str slice_end(const char* end) const { return slice((size_t)0, end); }
};

}
