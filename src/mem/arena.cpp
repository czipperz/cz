#include "arena.hpp"

#include <memory>
#include <string.h>
#include "allocator.hpp"
#include "global_allocator.hpp"
#include "../assert.hpp"

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
    global_allocator.dealloc(start, end - start);
}

static void* advance_ptr_to_alignment(void* old_ptr, size_t old_size, AllocInfo info) {
    return std::align(info.alignment, info.size, old_ptr, old_size);
}

static void* arena_realloc(void* _arena,
                           void* old_ptr,
                           size_t old_size,
                           AllocInfo info) {
    auto arena = static_cast<Arena*>(_arena);
    void* new_ptr = advance_ptr_to_alignment(old_ptr, old_size, info);
    if (info.size <= old_size) {
        return old_ptr;
    } else {
        new_ptr = alloc(arena, info);
        memcpy(new_ptr, old_ptr, old_size);
        return new_ptr;
    }
}

Allocator Arena::allocator() {
    return {
        arena_realloc,
        this,
    };
}

}
}
