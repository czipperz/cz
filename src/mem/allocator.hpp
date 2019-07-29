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

    /// Allocate memory using this allocator.
    void* alloc(size_t size);

    /// Deallocate memory allocated using this allocator.
    void dealloc(void* ptr, size_t size);

    /// Reallocate memory allocated using this allocator.
    void* realloc(void* old_ptr, size_t old_size, size_t new_size);
};

}
}
