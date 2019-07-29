#include "allocator.hpp"
#include <stdlib.h>

namespace cz {
void* alloc(size_t size) {
    return allocator.alloc(size);
}

void dealloc(void* ptr, size_t size) {
    return allocator.dealloc(ptr, size);
}

void* realloc(void* ptr, size_t size, size_t new_size) {
    return allocator.realloc(ptr, size, new_size);
}

void* Allocator::run(void* old_ptr, size_t old_size, size_t new_size) {
    return allocate(data, old_ptr, old_size, new_size);
}

void* Allocator::alloc(size_t size) {
    return run(NULL, 0, size);
}

void Allocator::dealloc(void* ptr, size_t size) {
    run(ptr, size, 0);
}

void* Allocator::realloc(void* ptr, size_t size, size_t new_size) {
    return run(ptr, size, new_size);
}

Allocator allocator = memory_allocator();

Allocator memory_allocator() {
    return {
        memory_allocate,
        NULL
    };
}

void* memory_allocate(void*, void* old_ptr, size_t old_size, size_t new_size) {
    if (old_size == 0) {
        return malloc(new_size);
    } else {
        return ::realloc(old_ptr, new_size);
    }
}
}
