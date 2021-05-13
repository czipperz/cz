#include <cz/allocator.hpp>

#include <stdlib.h>

namespace cz {

#ifndef NDEBUG
static const unsigned char alloc_fill = 0xCD;
static const unsigned char dealloc_fill = 0xDD;

void* Allocator::alloc(AllocInfo new_info) const {
    CZ_DEBUG_ASSERT(new_info.alignment > 0);

    void* ptr = func(data, {}, new_info);
    if (ptr) {
        memset(ptr, alloc_fill, new_info.size);
    }
    return ptr;
}

void Allocator::dealloc(MemSlice old_mem) const {
    if (old_mem.buffer) {
        memset(old_mem.buffer, dealloc_fill, old_mem.size);
    }
    func(data, old_mem, {0, 0});
}

void* Allocator::realloc(MemSlice old_mem, AllocInfo new_info) const {
    CZ_DEBUG_ASSERT(new_info.alignment > 0);

    if (old_mem.buffer && new_info.size < old_mem.size) {
        // TODO: This invalidates the memory if the realloc fails.  I don't think
        // this matters in practice but it might be something to look out for.
        memset((char*)old_mem.buffer + new_info.size, dealloc_fill, old_mem.size - new_info.size);
    }

    void* ptr = func(data, old_mem, new_info);
    if (ptr && new_info.size > old_mem.size) {
        memset((char*)ptr + old_mem.size, alloc_fill, new_info.size - old_mem.size);
    }
    return ptr;
}
#endif

}
