#pragma once

#include <stddef.h>
#include <cstddef>
#include "../context_decl.hpp"
#include "allocator.hpp"

namespace cz {
namespace mem {

struct Arena {
    constexpr Arena() = default;
    constexpr explicit Arena(MemSlice mem) : mem(mem) {}

    static const size_t alignment = alignof(std::max_align_t);

    MemSlice mem;
    size_t offset = 0;

    /// Create an \c cz::Allocator allocating memory in the \c Arena.
    Allocator allocator();

    MemSlice remaining() const { return {point(), mem.size - offset}; }
    void* point() const { return (char*)mem.buffer + offset; }
    void set_point(void* p) { offset = (char*)p - (char*)mem.buffer; }
};

template <size_t size>
struct StackArena : public Arena {
    alignas(Arena::alignment) char buffer[size];

    constexpr StackArena() : Arena(this->buffer) {}
};

struct HeapArena : public Arena {
    explicit HeapArena(Allocator, AllocInfo info);

    void drop(Allocator);
};

}
}
