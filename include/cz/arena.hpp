#pragma once

#include <stddef.h>
#include "allocator.hpp"

namespace cz {

/// An `Arena` splits up a memory buffer into a series of allocations.
/// Allocating memory in the arena is as simple as just incrementing the pointer.
///
/// Deallocation isn't 100% foolproof because of the 0 overhead approach.
/// Deallocating the most recently allocated entry will always succeed.  But
/// deallocating after that point will only suceed if all allocations in the
/// arena have the same alignment.  If deallocation fails then nothing happens.
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

    Allocator allocator() { return {Arena::realloc, Arena::dealloc, this}; }
    size_t remaining() const { return end - pointer; }

    static void* realloc(void* arena, MemSlice old_mem, AllocInfo new_info);
    static void dealloc(void* arena, MemSlice old_mem);
};

}
