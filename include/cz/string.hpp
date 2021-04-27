#pragma once

#include <stddef.h>
#include "allocator.hpp"
#include "str.hpp"

namespace cz {

struct String {
    char* _buffer;
    size_t _len;
    size_t _cap;

    /// Reserve `extra` extra spaces in the buffer.  Ensures `cap() >= len() + extra`.
    void reserve(Allocator allocator, size_t extra) {
        return reserve_total(allocator, _len + extra);
    }
    /// Reserve `total` total spaces in the buffer.  Ensures `cap() >= total`.
    void reserve_total(Allocator, size_t total);

    /// Push character `ch` onto the end of the string.
    ///
    /// Panics if there isn't enough space.
    void push(char ch) { append({&ch, 1}); }
    /// Calls `push` `count` times.
    void push_many(char ch, size_t count);
    /// Append the string `str` to the buffer.
    ///
    /// Panics if there isn't enough space.
    void append(Str str);

    /// Push `'\0'` onto the end of the string without changing the length.
    ///
    /// Panics if there isn't enough space.
    void null_terminate();

    /// Insert the character `ch` into the middle of the buffer.
    ///
    /// Panics if there isn't enough space or if `index > len()`.
    void insert(size_t index, char ch) { insert(index, {&ch, 1}); }
    /// Insert the string `str` into the middle of the buffer.
    ///
    /// Panics if there isn't enough space or if `index > len()`.
    void insert(size_t index, Str str);

    /// Removes `count` characters start at `index`.
    ///
    /// Panics if there isn't enough space or if `index + count > len()`.
    void remove(size_t index, size_t count);

    /// Pop the last character off the string.
    ///
    /// Panics if `len() == 0`.
    char pop();

    /// Reallocate the buffer so that the length is the same as the capacity.
    ///
    /// If the reallocation fails, nothing happens.
    void realloc(Allocator);

    /// Reallocate the buffer so that the capacity is one greater than the length.
    ///
    /// Panics if the reallocation fails.
    void realloc_null_terminate(Allocator);

    /// Set `len()` to `new_len`.  Panics if `new_len > cap()`.
    void set_len(size_t new_len);

    /// Create a new `String` with the same contents in a unique memory buffer.
    String clone(Allocator allocator) const { return as_str().duplicate(allocator); }
    String clone_null_terminate(Allocator allocator) const {
        return as_str().duplicate_null_terminate(allocator);
    }

    /// Dealloc the `String`.
    void drop(Allocator);

    /// Get the byte buffer backing the string.
    char* buffer();
    /// Get the byte buffer backing the string.
    const char* buffer() const;
    /// Get the length of the string in bytes.
    size_t len() const;
    /// Get the capacity of the string in bytes.
    size_t cap() const;

    char* start() { return buffer(); }
    const char* start() const { return buffer(); }
    char* end() { return buffer() + len(); }
    const char* end() const { return buffer() + len(); }

    char first() const {
        CZ_DEBUG_ASSERT(_len > 0);
        return _buffer[0];
    }
    char last() const {
        CZ_DEBUG_ASSERT(_len > 0);
        return _buffer[_len - 1];
    }

    bool equals_case_insensitive(Str prefix) const {
        return as_str().equals_case_insensitive(prefix);
    }

    bool starts_with(Str prefix) const { return as_str().starts_with(prefix); }
    bool ends_with(Str postfix) const { return as_str().ends_with(postfix); }

    bool starts_with_case_insensitive(Str prefix) const {
        return as_str().starts_with_case_insensitive(prefix);
    }
    bool ends_with_case_insensitive(Str postfix) const {
        return as_str().ends_with_case_insensitive(postfix);
    }

    size_t count(char c) const { return as_str().count(c); }

    bool contains(Str infix) const { return as_str().contains(infix); }
    bool contains(char infix) const { return as_str().contains(infix); }

    char* find(Str pattern) { return const_cast<char*>(as_str().find(pattern)); }
    const char* find(Str pattern) const { return as_str().find(pattern); }

    char* find(char pattern) { return const_cast<char*>(as_str().find(pattern)); }
    const char* find(char pattern) const { return as_str().find(pattern); }

    char* rfind(Str pattern) { return const_cast<char*>(as_str().rfind(pattern)); }
    const char* rfind(Str pattern) const { return as_str().rfind(pattern); }

    char* rfind(char pattern) { return const_cast<char*>(as_str().rfind(pattern)); }
    const char* rfind(char pattern) const { return as_str().rfind(pattern); }

    char* find_case_insensitive(Str pattern) {
        return const_cast<char*>(as_str().find_case_insensitive(pattern));
    }
    const char* find_case_insensitive(Str pattern) const {
        return as_str().find_case_insensitive(pattern);
    }

    char* find_case_insensitive(char pattern) {
        return const_cast<char*>(as_str().find_case_insensitive(pattern));
    }
    const char* find_case_insensitive(char pattern) const {
        return as_str().find_case_insensitive(pattern);
    }

    char* rfind_case_insensitive(Str pattern) {
        return const_cast<char*>(as_str().rfind_case_insensitive(pattern));
    }
    const char* rfind_case_insensitive(Str pattern) const {
        return as_str().rfind_case_insensitive(pattern);
    }

    char* rfind_case_insensitive(char pattern) {
        return const_cast<char*>(as_str().rfind_case_insensitive(pattern));
    }
    const char* rfind_case_insensitive(char pattern) const {
        return as_str().rfind_case_insensitive(pattern);
    }

    cz::Str slice(size_t start, size_t end) const { return as_str().slice(start, end); }
    cz::Str slice(const char* start, size_t end) const { return as_str().slice(start, end); }
    cz::Str slice(size_t start, const char* end) const { return as_str().slice(start, end); }
    cz::Str slice(const char* start, const char* end) const { return as_str().slice(start, end); }

    cz::Str slice_start(size_t start) const { return as_str().slice_start(start); }
    cz::Str slice_start(const char* start) const { return as_str().slice_start(start); }

    cz::Str slice_end(size_t end) const { return as_str().slice_end(end); }
    cz::Str slice_end(const char* end) const { return as_str().slice_end(end); }

    /// Get a `Str` representing this `String` in its current state.
    Str as_str() const;
    /// See `String::as_str()`.
    operator Str() const { return as_str(); }

    bool operator==(const Str& other) const { return this->as_str() == other; }
    bool operator!=(const Str& other) const { return this->as_str() != other; }
    bool operator<(const Str& other) const { return this->as_str() < other; }
    bool operator>(const Str& other) const { return this->as_str() > other; }
    bool operator<=(const Str& other) const { return this->as_str() <= other; }
    bool operator>=(const Str& other) const { return this->as_str() >= other; }

    char operator[](size_t i) const {
        CZ_DEBUG_ASSERT(i < len());
        return buffer()[i];
    }
    char& operator[](size_t i) {
        CZ_DEBUG_ASSERT(i < len());
        return buffer()[i];
    }
};

}
