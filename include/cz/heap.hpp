#pragma once

#include <stdlib.h>
#include "allocator.hpp"
#include "assert.hpp"

#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#else
#define TracyAlloc(x, y)
#define TracyFree(x)
#define ZoneScoped
#endif

namespace cz {

inline void* heap_allocator_realloc(void*, MemSlice old_mem, AllocInfo new_info) {
    ZoneScoped;

    // `realloc` can't be called with `size = 0` so handle it manually.
    if (new_info.size == 0) {
        free(old_mem.buffer);
        TracyFree(old_mem.buffer);
        return nullptr;
    }

    CZ_DEBUG_ASSERT(new_info.alignment <= alignof(max_align_t));
    void* ptr = realloc(old_mem.buffer, new_info.size);
    if (ptr) {
        TracyFree(old_mem.buffer);
        TracyAlloc(ptr, new_info.size);
    }
    return ptr;
}

/// Make an allocator that allocates memory in the heap.
inline Allocator heap_allocator() {
    return {heap_allocator_realloc, nullptr};
}

}
