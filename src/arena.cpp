#include <cz/arena.hpp>

#include <string.h>
#include <cz/alloc_utils.hpp>
#include <cz/allocator.hpp>
#include <cz/assert.hpp>

namespace cz {

void* Arena::realloc(void* _arena, MemSlice old_mem, AllocInfo new_info) {
    Arena* arena = (Arena*)_arena;
    CZ_DEBUG_ASSERT(arena->start != nullptr);
    CZ_DEBUG_ASSERT(arena->pointer >= arena->start);
    CZ_DEBUG_ASSERT(arena->pointer <= arena->end);

    if (old_mem.end() == arena->pointer) {
        // Realloc in place.
        MemSlice current = {old_mem.buffer, (size_t)(arena->end - (char*)old_mem.buffer)};
        void* ptr = advance_ptr_to_alignment(current, new_info);
        if (ptr) {
            arena->pointer = (char*)ptr + new_info.size;
        }
        return ptr;
    } else {
        // Ignore the dealloc and just allocate.
        MemSlice current = {arena->pointer, (size_t)(arena->end - arena->pointer)};
        void* ptr = advance_ptr_to_alignment(current, new_info);
        if (ptr) {
            arena->pointer = (char*)ptr + new_info.size;
            memcpy(ptr, old_mem.buffer, old_mem.size);
        }
        return ptr;
    }
}

}
