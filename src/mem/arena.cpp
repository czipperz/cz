#include "arena.hpp"

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

void* Arena::alloc(size_t size) {
    CZ_DEBUG_ASSERT(start != NULL);
    if (point + size <= end) {
        void* result = point;
        point += size;
        return result;
    } else {
        return NULL;
    }
}

void Arena::drop() {
    dealloc(start, end - start);
}

static void* arena_realloc(void* _arena,
                           void* old_ptr,
                           size_t old_size,
                           size_t new_size) {
    auto arena = static_cast<Arena*>(_arena);
    if (new_size <= old_size) {
        return old_ptr;
    } else {
        void* new_ptr = arena->alloc(new_size);
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
