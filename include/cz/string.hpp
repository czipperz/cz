#pragma once

#include <stddef.h>
#include "mem/allocator.hpp"
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
    void reserve(mem::Allocator, size_t extra);

    /// Append the \c Str to the buffer.
    ///
    /// Panics if there isn't enough space.
    void append(Str str);
    /// Insert the \c Str into the middle of the buffer.
    ///
    /// Panics if there isn't enough space or if \c index is greater than \c len.
    void insert(size_t index, Str str);

    /// Reallocate the buffer so that the length is the same as the capacity.
    ///
    /// If the reallocation fails, nothing happens.
    void realloc(mem::Allocator);

    /// Set the \c len to \c 0.
    void clear();
    /// Set the \c len to \c new_len.  Panics if \c new_len is greater than \c len.
    void shrink_to(size_t new_len);
    /// Set the \c len to \c new_len.  Panics if \c new_len is greater than \c cap.
    void set_len(size_t new_len);

    /// Create a new \c String with the same contents in a unique memory buffer.
    String clone(mem::Allocator allocator) const { return as_str().duplicate(allocator); }

    /// Dealloc the \c buffer.
    void drop(mem::Allocator);

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

    char operator[](size_t i) const { return buffer()[i]; }
    char& operator[](size_t i) { return buffer()[i]; }
};

}
