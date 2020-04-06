#pragma once

#include <stddef.h>

namespace cz {

template <size_t size, size_t alignment = alignof(max_align_t)>
struct Aligned_Buffer {
    alignas(alignment) char buffer[size];

    MemSlice mem() { return {buffer, size}; }
    operator MemSlice() { return mem(); }
};

}
