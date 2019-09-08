#pragma once

#include <stddef.h>
#include "../context_decl.hpp"
#include "allocator.hpp"

namespace cz {
namespace mem {

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

template <size_t size, size_t alignment = alignof(max_align_t)>
struct AlignedBuffer {
    alignas(alignment) char buffer[size];

    MemSlice mem() { return {buffer, size}; }
    operator MemSlice() { return mem(); }
};

}
}
