#include "arena.hpp"

#include <string.h>
#include "allocator.hpp"
#include "global_allocator.hpp"
#include "../assert.hpp"

namespace cz {
namespace mem {

Arena Arena::sized(size_t size) {
    return Arena(cz::mem::alloc(size), size);
}

Arena::Arena(void* _buffer, size_t size) {
    char* buffer = static_cast<char*>(_buffer);
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

void* Arena::realloc(void* old_ptr, size_t old_size, size_t new_size) {
    if (new_size <= old_size) {
        return old_ptr;
    } else {
        void* new_ptr = this->alloc(new_size);
        memcpy(new_ptr, old_ptr, old_size);
        return new_ptr;
    }
}

void Arena::drop() {
    dealloc(start, end - start);
}

static void* arena_realloc(void* arena,
                           void* old_ptr,
                           size_t old_size,
                           size_t new_size) {
    return static_cast<Arena*>(arena)->realloc(old_ptr, old_size, new_size);
}

Allocator Arena::allocator() {
    return {
        arena_realloc,
        this,
    };
}

}
}
