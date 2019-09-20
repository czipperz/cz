#pragma once

#include <stddef.h>
#include "allocator.hpp"
#include "str.hpp"

namespace cz {

class String {
    char* _buffer;
    size_t _len;
    size_t _cap;

public:
    /// Create a new empty \c String.
    String();
    /// Create a new \c String using the \c buffer with initial length \c len and capacity \c cap.
    explicit String(char* buffer, size_t len, size_t cap);

    /// Ensure there are \c extra bytes available in the buffer.
    void reserve(Allocator, size_t extra);

    /// Push the \c char onto the end of the string.
    ///
    /// Panics if there isn't enough space.
    void push(char ch) { append({&ch, 1}); }
    /// Append the \c Str to the buffer.
    ///
    /// Panics if there isn't enough space.
    void append(Str str);

    /// Push \c '\0' onto the end of the string without changing the length.
    ///
    /// Panics if there isn't enough space.
    void null_terminate();

    /// Insert the \c char into the middle of the buffer.
    ///
    /// Panics if there isn't enough space or if \c index is greater than \c len.
    void insert(size_t index, char ch) { insert(index, {&ch, 1}); }
    /// Insert the \c Str into the middle of the buffer.
    ///
    /// Panics if there isn't enough space or if \c index is greater than \c len.
    void insert(size_t index, Str str);

    /// Pop the last \c char off the string.
    ///
    /// Panics if there is no character.
    char pop();

    /// Reallocate the buffer so that the length is the same as the capacity.
    ///
    /// If the reallocation fails, nothing happens.
    void realloc(Allocator);

    /// Set the \c len to \c new_len.  Panics if \c new_len is greater than \c cap.
    void set_len(size_t new_len);

    /// Create a new \c String with the same contents in a unique memory buffer.
    String clone(Allocator allocator) const { return as_str().duplicate(allocator); }

    /// Dealloc the \c buffer.
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

    bool starts_with(Str prefix) const { return as_str().starts_with(prefix); }
    bool ends_with(Str postfix) const { return as_str().ends_with(postfix); }

    char* find(char pattern) { return const_cast<char*>(as_str().find(pattern)); }
    const char* find(char pattern) const { return as_str().find(pattern); }

    char* rfind(char pattern) { return const_cast<char*>(as_str().rfind(pattern)); }
    const char* rfind(char pattern) const { return as_str().rfind(pattern); }

    /// Get a \c Str representing this \c String in its current state.
    Str as_str() const;
    /// See \c String::as_str().
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

struct AllocatedString : String {
    Allocator allocator;

    void reserve(size_t extra) { String::reserve(allocator, extra); }
    void realloc() { String::realloc(allocator); }
    void drop() { String::drop(allocator); }
};

}
