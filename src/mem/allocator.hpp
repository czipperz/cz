#pragma once

#include <stddef.h>
#include "../context_decl.hpp"
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
using Allocate = MemSlice (*)(C* c, void* data, MemSlice old_mem, AllocInfo new_info);

/// A memory allocator.
struct Allocator {
    Allocate allocate;
    void* data;

    /// Allocate memory using this allocator.
    MemSlice alloc(C* c, AllocInfo info) const {
        return realloc(c, {}, info);
    }

    /// Allocate memory to store a value of the given type.
    template <class T>
    T* alloc(C* c) const {
        return (T*)alloc(c, alloc_info<T>()).buffer;
    }

    /// Deallocate memory allocated using this allocator.
    void dealloc(C* c, MemSlice mem) const {
        realloc(c, mem, {});
    }

    /// Reallocate memory allocated using this allocator.
    MemSlice realloc(C* c, MemSlice old_mem, AllocInfo new_info) const {
        return allocate(c, data, old_mem, new_info);
    }
};

}
}
