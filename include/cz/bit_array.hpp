#pragma once

#include <limits.h>
#include <stddef.h>
#include "allocator.hpp"
#include "assert.hpp"

namespace cz {

/// A bit array provides an easy to use interface to store many bools as individual bits.
struct Bit_Array {
    unsigned char* buffer;

    /// The number of bytes required to store `len` bits.
    static size_t alloc_size(size_t len) { return (len + CHAR_BIT - 1) / CHAR_BIT; }

    /// Initialize the bit array by allocating enough space for `len` bits.
    void init(cz::Allocator allocator, size_t len) {
        buffer = allocator.alloc_zeroed<unsigned char>(alloc_size(len));
        CZ_ASSERT(buffer);
    }

    /// Deallocate the associated memory.
    void drop(cz::Allocator allocator, size_t len) { allocator.dealloc(buffer, alloc_size(len)); }

    /// Turn on the bit at `index`.
    void set(size_t index) {
        size_t byte = index / CHAR_BIT;
        size_t bit = index % CHAR_BIT;
        unsigned char* p = &buffer[byte];
        *p = *p | (1 << bit);
    }

    /// Turn off the bit at `index`.
    void unset(size_t index) {
        size_t byte = index / CHAR_BIT;
        size_t bit = index % CHAR_BIT;
        unsigned char* p = &buffer[byte];
        *p = *p & ~(1 << bit);
    }

    /// Turn off all bits.  `len` must match the `len` passed into `init`.
    void clear(size_t len) {
        memset(buffer, 0, alloc_size(len));
    }

    /// Get the bit at `index`.
    bool get(size_t index) const {
        size_t byte = index / CHAR_BIT;
        size_t bit = index % CHAR_BIT;
        const unsigned char* p = &buffer[byte];
        return *p & (1 << bit);
    }
};

/// A bit array that stores its own length.  This is useful if calculating the length is costly or
/// if you're using the bit array as a function-local variable to make it easier to deallocate.
struct Sized_Bit_Array : Bit_Array {
    size_t len;

    /// Initialize the sized bit array and set the `len` field to `len_`.
    void init(cz::Allocator allocator, size_t len_) {
        len = len_;
        Bit_Array::init(allocator, len);
    }

    /// Deallocate the associated memory.
    void drop(cz::Allocator allocator) { Bit_Array::drop(allocator, len); }

    /// Turn off all bits.
    void clear() {
        clear(len);
    }
};

}
