#pragma once

#include <stddef.h>

namespace cz {
/// Allocate, deallocate, or reallocate memory.
///
/// If \c old_size is \c 0 it can be assumed the pointer is \c NULL.
/// If \c new_size is \c 0 the user is trying to deallocate memory.
typedef void* (*Allocate)(void* data,
                          void* old_ptr,
                          size_t old_size,
                          size_t new_size);

struct Allocator {
    Allocate allocate;
    void* data;

    void* run(void* old_ptr, size_t old_size, size_t new_size);
    void* alloc(size_t size);
    void dealloc(void* ptr, size_t size);
    void* realloc(void* ptr, size_t size, size_t new_size);
};

extern Allocator allocator;

void* alloc(size_t size);
void dealloc(void* ptr, size_t size);
void* realloc(void* ptr, size_t size, size_t new_size);

Allocator memory_allocator();
/// Allocate memory.
///
/// The data parameter is ignored.
void* memory_allocate(void* data, void* old_ptr, size_t old_size, size_t new_size);
}
