#include <cz/arena.hpp>

#include <string.h>
#include <cz/alloc_utils.hpp>
#include <cz/allocator.hpp>
#include <cz/assert.hpp>

namespace cz {

static void* arena_alloc(void* _arena, AllocInfo info) {
    Arena* arena = static_cast<Arena*>(_arena);
    CZ_DEBUG_ASSERT(arena->mem.buffer != nullptr);

    void* result = advance_ptr_to_alignment(arena->remaining(), info);
    if (result) {
        arena->set_point((char*)result + info.size);
    }
    return result;
}

static void arena_dealloc(void* _arena, MemSlice old_mem) {
    Arena* arena = static_cast<Arena*>(_arena);
    CZ_DEBUG_ASSERT(arena->mem.buffer != nullptr);
    CZ_DEBUG_ASSERT(arena->mem.contains(old_mem));

    if (arena->point() == old_mem.end()) {
        CZ_DEBUG_ASSERT(arena->mem.contains(old_mem));
        arena->set_point(old_mem.start());
    }
}

static void* arena_realloc(void* _arena, MemSlice old_mem, AllocInfo new_info) {
    Arena* arena = static_cast<Arena*>(_arena);
    CZ_DEBUG_ASSERT(arena->mem.buffer != nullptr);
    CZ_DEBUG_ASSERT(arena->mem.contains(old_mem));

    if (arena->point() == old_mem.end()) {
        // Pretend we dealloc the memory then allocate again so we get more space.
        size_t remaining_size = (char*)arena->mem.end() - (char*)old_mem.buffer;
        void* old_aligned = advance_ptr_to_alignment({old_mem.buffer, remaining_size}, new_info);

        if (old_aligned) {
            // Move the memory to deal with alignment.
            memmove(old_aligned, old_mem.buffer, new_info.size);
            arena->set_point((char*)old_aligned + new_info.size);
        }
        return old_aligned;
    } else {
        auto old_aligned = advance_ptr_to_alignment(old_mem, new_info);
        if (old_aligned) {
            // Allocate as a subset of old_mem
            memmove(old_aligned, old_mem.buffer, new_info.size);
            return old_aligned;
        } else {
            // Allocate a fresh copy
            void* new_mem = arena_alloc(arena, new_info);
            if (new_mem && old_mem.buffer) {
                // Must have a greater new size as smaller sizes are handled above
                CZ_DEBUG_ASSERT(new_info.size <= old_mem.size);
                memcpy(new_mem, old_mem.buffer, old_mem.size);
            }
            return new_mem;
        }
    }
}

Allocator Arena::allocator() {
    static const Allocator::VTable vtable = {arena_alloc, arena_dealloc, arena_realloc};
    return {&vtable, this};
}

}
