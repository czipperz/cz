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

inline void* heap_allocator_alloc(void*, AllocInfo info) {
    CZ_DEBUG_ASSERT(info.alignment <= alignof(max_align_t));
    void* ptr = malloc(info.size);
    TracyAlloc(ptr, info.size);
    return ptr;
}

inline void heap_allocator_dealloc(void*, MemSlice mem) {
    TracyFree(mem.buffer);
    free(mem.buffer);
}

inline void* heap_allocator_realloc(void*, MemSlice old_mem, AllocInfo new_info) {
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
    static const Allocator::VTable vtable = {heap_allocator_alloc, heap_allocator_dealloc,
                                             heap_allocator_realloc};
    return {&vtable, nullptr};
}

}
