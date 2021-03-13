#pragma once

#include <stdlib.h>
#include "assert.hpp"
#include "allocator.hpp"

namespace cz {

inline void* heap_allocator_alloc(void*, AllocInfo info) {
    CZ_DEBUG_ASSERT(info.alignment <= alignof(max_align_t));
    return malloc(info.size);
}

inline void heap_allocator_dealloc(void*, MemSlice mem) {
    free(mem.buffer);
}

inline void* heap_allocator_realloc(void*, MemSlice old_mem, AllocInfo new_info) {
    CZ_DEBUG_ASSERT(new_info.alignment <= alignof(max_align_t));
    return realloc(old_mem.buffer, new_info.size);
}

/// Make an allocator that allocates memory in the heap.
inline Allocator heap_allocator() {
    static const Allocator::VTable vtable = {heap_allocator_alloc, heap_allocator_dealloc,
                                             heap_allocator_realloc};
    return {&vtable, nullptr};
}

}
