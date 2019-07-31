#include "arena.hpp"

#include <string.h>
#include <memory>
#include "../assert.hpp"
#include "allocator.hpp"
#include "global_allocator.hpp"

namespace cz {
namespace mem {

Arena::Arena(void* _buffer, size_t size) {
    auto buffer = static_cast<char*>(_buffer);
    start = buffer;
    point = buffer;
    end = buffer + size;
}

Arena::Arena() {
#ifndef NDEBUG
    start = NULL;
    point = NULL;
    end = NULL;
#endif
}

static void* alloc(Arena* arena, AllocInfo info) {
    CZ_DEBUG_ASSERT(arena->start != NULL);
    if (arena->point + info.size <= arena->end) {
        void* result = arena->point;
        arena->point += info.size;
        return result;
    } else {
        return NULL;
    }
}

void Arena::drop() {
    global_allocator.dealloc({start, static_cast<size_t>(end - start)});
}

static void* advance_ptr_to_alignment(MemSlice old_mem, AllocInfo new_info) {
    // std::align uses references to modify the old variables inplace so is
    // difficult to correctly inline.  This returns true if there is enough room
    // after aligning.
    return std::align(new_info.alignment, new_info.size, old_mem.buffer, old_mem.len);
}

static void* arena_realloc(void* _arena, MemSlice old_mem, AllocInfo new_info) {
    auto arena = static_cast<Arena*>(_arena);
    auto new_ptr = advance_ptr_to_alignment(old_mem, new_info);
    if (new_ptr) {
        return new_ptr;
    }

    new_ptr = alloc(arena, new_info);
    memcpy(new_ptr, old_mem.buffer, old_mem.len);
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
