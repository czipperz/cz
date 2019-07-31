#pragma once

#include <stddef.h>
#include "alloc_info.hpp"

namespace cz {
namespace mem {

/// Allocate, deallocate, or reallocate memory.
///
/// If \c old_size is \c 0 it can be assumed the pointer is \c NULL.
/// If \c new_size is \c 0 the user is trying to deallocate memory.
typedef void* (*Allocate)(void* data, void* old_ptr, size_t old_size, AllocInfo new_info);

/// A memory allocator.
struct Allocator {
    Allocate allocate;
    void* data;

    /// Allocate memory using this allocator.
    void* alloc(AllocInfo new_info);

    /// Allocate memory to store a value of the given type.
    template <class T>
    T* alloc() {
        return (T*)alloc(alloc_info<T>());
    }

    /// Deallocate memory allocated using this allocator.
    void dealloc(void* ptr, size_t size);

    /// Reallocate memory allocated using this allocator.
    void* realloc(void* old_ptr, size_t old_size, AllocInfo new_info);

    /// Reallocate memory alloceted using this allocator to store a value of the
    /// given type.
    template <class T>
    T* realloc(void* old_ptr, size_t old_size) {
        return (T*)realloc(old_ptr, old_size, alloc_info<T>());
    }
};

}
}
