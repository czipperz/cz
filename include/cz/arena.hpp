#pragma once

#include <stddef.h>
#include "allocator.hpp"

namespace cz {

struct Arena {
    MemSlice mem;
    size_t offset = 0;

    constexpr Arena() = default;
    constexpr explicit Arena(MemSlice mem) : mem(mem) {}

    /// Create an \c cz::Allocator allocating memory in the \c Arena.
    Allocator allocator();

    MemSlice remaining() const { return {point(), mem.size - offset}; }
    void* point() const { return (char*)mem.buffer + offset; }
    void set_point(void* p) { offset = (char*)p - (char*)mem.buffer; }
};

}
