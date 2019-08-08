#pragma once

#include <stddef.h>
#include "../slice.hpp"
#include "alloc_info.hpp"

namespace cz {
namespace mem {

/// Allocate, deallocate, or reallocate memory.
///
/// If \c old_mem.size is \c 0 it can be assumed that \c old_mem.buffer is \c NULL.
///
/// If \c old_mem.buffer is \c NULL the user is trying to allocate memory.
/// If \c new_info.size is \c 0 the user is trying to deallocate memory.
struct Allocate {
    MemSlice (*alloc)(void* data, AllocInfo new_info);
    void (*dealloc)(void* data, MemSlice old_mem);
    MemSlice (*realloc)(void* data, MemSlice old_mem, AllocInfo new_info);
};

/// A memory allocator.
struct Allocator {
    Allocate allocate;
    void* data;

    /// Allocate memory using this allocator.
    MemSlice alloc(AllocInfo info) const { return allocate.alloc(data, info); }

    /// Allocate memory to store a value of the given type using this allocator.
    template <class T>
    T* alloc() const {
        return (T*)alloc(alloc_info<T>()).buffer;
    }

    /// Deallocate memory allocated using this allocator.
    void dealloc(MemSlice mem) const { return allocate.dealloc(data, mem); }

    /// Reallocate a section of memory allocated using this allocator.
    MemSlice realloc(MemSlice old_mem, AllocInfo new_info) const {
        return allocate.realloc(data, old_mem, new_info);
    }
};

}
}
