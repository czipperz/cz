#include "heap.hpp"

#include <stdlib.h>

namespace cz {
namespace mem {

static MemSlice heap_allocate(C*, void*, MemSlice old_mem, AllocInfo new_info) {
    // TODO make alignment work
    if (old_mem.size == 0) {
        return {malloc(new_info.size), new_info.size};
    } else {
        return {::realloc(old_mem.buffer, new_info.size), new_info.size};
    }
}

Allocator heap_allocator() {
    return {heap_allocate, NULL};
}

}
}
