#include "arena.hpp"

#include <string.h>
#include "../assert.hpp"
#include "../context.hpp"
#include "alloc_utils.hpp"
#include "allocator.hpp"

namespace cz {
namespace mem {

static MemSlice arena_alloc(void* _arena, AllocInfo info) {
    auto arena = static_cast<Arena*>(_arena);
    CZ_DEBUG_ASSERT(arena->mem.buffer != NULL);

    void* result = advance_ptr_to_alignment(arena->remaining(), info);
    if (result) {
        MemSlice new_mem = {result, info.size};
        arena->set_point(new_mem.end());
        return new_mem;
    } else {
        return {NULL, 0};
    }
}

static void arena_dealloc(void* _arena, MemSlice old_mem) {
    auto arena = static_cast<Arena*>(_arena);
    CZ_DEBUG_ASSERT(arena->mem.buffer != NULL);

    if (arena->point() == old_mem.end()) {
        CZ_DEBUG_ASSERT(arena->mem.contains(old_mem));
        arena->set_point(old_mem.start());
    }
}

static MemSlice arena_realloc(void* _arena, MemSlice old_mem, AllocInfo new_info) {
    auto arena = static_cast<Arena*>(_arena);
    CZ_DEBUG_ASSERT(arena->mem.buffer != NULL);

    if (arena->point() == old_mem.end()) {
        // Pretend we dealloc the memory then allocate again so we get more space.
        size_t remaining_size = (char*)arena->mem.end() - (char*)old_mem.buffer;
        auto old_aligned = advance_ptr_to_alignment({old_mem.buffer, remaining_size}, new_info);

        if (old_aligned) {
            // Move the memory to deal with alignment.
            memmove(old_aligned, old_mem.buffer, new_info.size);
            MemSlice new_mem = {old_aligned, new_info.size};
            arena->set_point(new_mem.end());
            return new_mem;
        } else {
            return {NULL, 0};
        }
    } else {
        auto old_aligned = advance_ptr_to_alignment(old_mem, new_info);
        if (old_aligned) {
            // Allocate as a subset of old_mem
            memmove(old_aligned, old_mem.buffer, new_info.size);
            return {old_aligned, new_info.size};
        } else {
            // Allocate a fresh copy
            auto new_mem = arena_alloc(arena, new_info);
            if (new_mem.buffer && old_mem.buffer) {
                // Must have a greater new size as smaller sizes are handled above
                CZ_DEBUG_ASSERT(new_info.size <= old_mem.size);
                memcpy(new_mem.buffer, old_mem.buffer, old_mem.size);
            }
            return new_mem;
        }
    }
}

Allocator Arena::allocator() {
    static const Allocator::VTable vtable = {arena_alloc, arena_dealloc, arena_realloc};
    return {&vtable, this};
}

HeapArena::HeapArena(Allocator allocator, AllocInfo info) {
    mem = allocator.alloc(info);
    CZ_ASSERT(mem.buffer != NULL);
}

void HeapArena::drop(Allocator allocator) {
    allocator.dealloc(mem);
}

}
}
