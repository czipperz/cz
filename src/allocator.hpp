#pragma once

#include <stddef.h>

namespace cz {
namespace mem {

/// Allocate, deallocate, or reallocate memory.
///
/// If \c old_size is \c 0 it can be assumed the pointer is \c NULL.
/// If \c new_size is \c 0 the user is trying to deallocate memory.
typedef void* (*Allocate)(void* data,
                          void* old_ptr,
                          size_t old_size,
                          size_t new_size);

/// A memory allocator.
struct Allocator {
    Allocate allocate;
    void* data;

    void* alloc(size_t size);
    void dealloc(void* ptr, size_t size);
    void* realloc(void* ptr, size_t size, size_t new_size);
};

/// The global memory allocator.
extern Allocator global_allocator;

/// Allocate memory using the global allocator.
void* alloc(size_t size);

/// Deallocate memory allocated using the global allocator.
void dealloc(void* ptr, size_t size);

/// Reallocate memory allocated using the global allocator.
void* realloc(void* ptr, size_t size, size_t new_size);

}
}
