#include <cz/heap.hpp>

#if defined(_WIN32) || _ISOC11_SOURCE
#include <malloc.h>
#else
#include <stdlib.h>
#endif

#include <cz/assert.hpp>
#include <cz/util.hpp>

namespace cz {

static void* heap_alloc(void*, AllocInfo info) {
#ifdef _WIN32
    return _aligned_malloc(info.size, info.alignment);
#elif _ISOC11_SOURCE || __cplusplus >= 201703L
    return aligned_alloc(info.alignment, info.size);
#elif _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
    void* buf;
    if (posix_memalign(&buf, max(info.alignment, alignof(void*)), info.size) == 0) {
        return buf;
    } else {
        return nullptr;
    }
#else
    CZ_DEBUG_ASSERT(info.alignment <= alignof(max_align_t));
    return buf = malloc(info.size);
#endif
}

static void heap_dealloc(void*, MemSlice mem) {
#ifdef _WIN32
    _aligned_free(mem.buffer);
#else
    free(mem.buffer);
#endif
}

static void* heap_realloc(void*, MemSlice old_mem, AllocInfo new_info) {
#ifdef _WIN32
    return _aligned_realloc(old_mem.buffer, new_info.size, new_info.alignment);
#else
    return realloc(old_mem.buffer, new_info.size);
#endif
}

Allocator heap_allocator() {
    static const Allocator::VTable vtable = {heap_alloc, heap_dealloc, heap_realloc};
    return {&vtable, nullptr};
}

}
