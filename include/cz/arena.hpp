#pragma once

#include <stddef.h>
#include "allocator.hpp"

namespace cz {

struct Arena {
    char* start;
    char* pointer;
    char* end;

    void init(MemSlice mem) { init(mem.buffer, mem.size); }
    void init(void* start, size_t size) {
        this->start = (char*)start;
        this->pointer = this->start;
        this->end = this->start + size;
    }

    void init(Allocator allocator, size_t size) {
        void* ptr = allocator.alloc({size, /*alignment=*/16});
        CZ_ASSERT(ptr);
        init(ptr, size);
    }

    void drop(Allocator allocator) { allocator.dealloc(start, end - start); }

    Allocator allocator() { return {Arena::realloc, this}; }
    size_t remaining() const { return end - pointer; }

    static void* realloc(void* arena, MemSlice old_mem, AllocInfo new_info);
};

}
