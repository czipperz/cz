#include "cz/mem/arena.hpp"

#include <string.h>
#include "cz/assert.hpp"
#include "cz/context.hpp"
#include "cz/mem/alloc_utils.hpp"
#include "cz/mem/allocator.hpp"

namespace cz {
namespace mem {

const size_t Arena::alignment;

static size_t zero_alignment(size_t x) {
    return x & ~(Arena::alignment - 1);
}

static size_t expand_to_alignment(size_t x) {
    return zero_alignment(x + Arena::alignment - 1);
}

static AllocInfo expand_to_alignment(AllocInfo info) {
    return {expand_to_alignment(info.size), expand_to_alignment(info.alignment)};
}

static MemSlice arena_alloc(void* _arena, AllocInfo info) {
    auto arena = static_cast<Arena*>(_arena);
    CZ_DEBUG_ASSERT(arena->mem.buffer != nullptr);

    info = expand_to_alignment(info);

    void* result = advance_ptr_to_alignment(arena->remaining(), info);
    if (result) {
        MemSlice new_mem = {result, info.size};
        arena->set_point(new_mem.end());
        return new_mem;
    } else {
        return {nullptr, 0};
    }
}

static void arena_dealloc(void* _arena, MemSlice old_mem) {
    auto arena = static_cast<Arena*>(_arena);
    CZ_DEBUG_ASSERT(arena->mem.buffer != nullptr);
    CZ_DEBUG_ASSERT(arena->mem.contains(old_mem));

    old_mem.size = expand_to_alignment(old_mem.size);

    if (arena->point() == old_mem.end()) {
        arena->set_point(old_mem.start());
    }
}

static MemSlice arena_realloc(void* _arena, MemSlice old_mem, AllocInfo new_info) {
    auto arena = static_cast<Arena*>(_arena);
    CZ_DEBUG_ASSERT(arena->mem.buffer != nullptr);
    CZ_DEBUG_ASSERT(arena->mem.contains(old_mem));

    old_mem.size = expand_to_alignment(old_mem.size);
    new_info = expand_to_alignment(new_info);

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
            return {nullptr, 0};
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

}
}
