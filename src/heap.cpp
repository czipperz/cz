#include <cz/heap.hpp>

#if defined(_WIN32) || _ISOC11_SOURCE
#include <malloc.h>
#else
#include <stdlib.h>
#endif

#include <cz/assert.hpp>
#include <cz/util.hpp>

namespace cz {

static MemSlice heap_alloc(void*, AllocInfo info) {
#ifdef _WIN32
    void* buf = _aligned_malloc(info.size, info.alignment);
#elif _ISOC11_SOURCE || __cplusplus >= 201703L
    void* buf = aligned_alloc(info.alignment, info.size);
#elif _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
    void* buf;
    if (posix_memalign(&buf, max(info.alignment, alignof(void*)), info.size) == 0) {
        return {buf, info.size};
    } else {
        return {nullptr, 0};
    }
#else
    CZ_DEBUG_ASSERT(info.alignment <= alignof(max_align_t));
    void* buf = malloc(info.size);
#endif

    if (buf) {
        return {buf, info.size};
    } else {
        return {nullptr, 0};
    }
}

static void heap_dealloc(void*, MemSlice mem) {
#ifdef _WIN32
    _aligned_free(mem.buffer);
#else
    free(mem.buffer);
#endif
}

static MemSlice heap_realloc(void*, MemSlice old_mem, AllocInfo new_info) {
#ifdef _WIN32
    void* buf = _aligned_realloc(old_mem.buffer, new_info.size, new_info.alignment);
#else
    void* buf = realloc(old_mem.buffer, new_info.size);
#endif
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
