#include <cz/mem/heap.hpp>

#include <stdlib.h>

namespace cz {
namespace mem {

static MemSlice heap_alloc(void*, AllocInfo info) {
    // TODO alignment
    auto buf = malloc(info.size);
    if (buf) {
        return {buf, info.size};
    } else {
        return {nullptr, 0};
    }
}

static void heap_dealloc(void*, MemSlice mem) {
    free(mem.buffer);
}

static MemSlice heap_realloc(void*, MemSlice old_mem, AllocInfo new_info) {
    // TODO alignment
    auto buf = realloc(old_mem.buffer, new_info.size);
    if (buf) {
        return {buf, new_info.size};
    } else {
        return {nullptr, 0};
    }
}

Allocator heap_allocator() {
    static const Allocator::VTable vtable = {heap_alloc, heap_dealloc, heap_realloc};
    return {&vtable, nullptr};
}

}
}
