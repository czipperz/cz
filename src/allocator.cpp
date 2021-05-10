#include <cz/allocator.hpp>

#include <stdlib.h>

namespace cz {

#ifndef NDEBUG
static const char alloc_fill = rand();
static const char dealloc_fill = rand();

void* Allocator::realloc(MemSlice old_mem, AllocInfo new_info) const {
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
