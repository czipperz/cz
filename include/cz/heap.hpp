#pragma once

#include <stdlib.h>
#include "allocator.hpp"
#include "assert.hpp"

#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#else
#define TracyAlloc(x, y)
#define TracyFree(x)
#endif

namespace cz {

inline void* heap_allocator_realloc(void*, MemSlice old_mem, AllocInfo new_info) {
    // Since it is undefined what happens when `std::realloc` is
    // called with `size = 0` we just always allocate 1 byte.
    if (new_info.size == 0) {
        new_info.size = 1;
    }

    // Allocation or reallocation.
    CZ_DEBUG_ASSERT(new_info.alignment <= alignof(max_align_t));
    void* ptr = realloc(old_mem.buffer, new_info.size);
    if (ptr) {
        TracyFree(old_mem.buffer);
        TracyAlloc(ptr, new_info.size);
    }
    return ptr;
}

inline void heap_allocator_dealloc(void*, MemSlice old_mem) {
    free(old_mem.buffer);
    TracyFree(old_mem.buffer);
}

/// Make an allocator that allocates memory in the heap.
inline Allocator heap_allocator() {
    return {heap_allocator_realloc, heap_allocator_dealloc, nullptr};
}

}
