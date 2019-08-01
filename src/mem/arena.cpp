#include "arena.hpp"

#include <string.h>
#include <memory>
#include "../assert.hpp"
#include "allocator.hpp"
#include "global_allocator.hpp"

namespace cz {
namespace mem {

static void* alloc(Arena* arena, AllocInfo info) {
    CZ_DEBUG_ASSERT(arena->mem.buffer != NULL);
    if (arena->offset + info.size <= arena->mem.size) {
        void* result = static_cast<char*>(arena->mem.buffer) + arena->offset;
        arena->offset += info.size;
        return result;
    } else {
        return NULL;
    }
}

void Arena::drop() {
    global_allocator.dealloc(mem);
}

static void* advance_ptr_to_alignment(MemSlice old_mem, AllocInfo new_info) {
    // std::align uses references to modify the old variables inplace so is
    // difficult to correctly inline.  This returns true if there is enough room
    // after aligning.
    return std::align(new_info.alignment, new_info.size, old_mem.buffer, old_mem.size);
}

static void* arena_realloc(void* _arena, MemSlice old_mem, AllocInfo new_info) {
    auto arena = static_cast<Arena*>(_arena);
    auto new_ptr = advance_ptr_to_alignment(old_mem, new_info);
    if (new_ptr) {
        return new_ptr;
    }

    new_ptr = alloc(arena, new_info);
    memcpy(new_ptr, old_mem.buffer, old_mem.size);
    return new_ptr;
}

Allocator Arena::allocator() {
    return {
        arena_realloc,
        this,
    };
}

}
}
