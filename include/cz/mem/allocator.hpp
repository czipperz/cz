#pragma once

#include <stddef.h>
#include "../slice.hpp"
#include "alloc_info.hpp"

namespace cz {
namespace mem {

/// A memory allocator.
struct Allocator {
    struct VTable {
        MemSlice (*alloc)(void* data, AllocInfo new_info);
        void (*dealloc)(void* data, MemSlice old_mem);
        MemSlice (*realloc)(void* data, MemSlice old_mem, AllocInfo new_info);
    };

    const Allocator::VTable* vtable;
    void* data;

    /// Allocate memory using this allocator.
    MemSlice alloc(AllocInfo info) const { return vtable->alloc(data, info); }

    /// Allocate memory to store a value of the given type using this allocator.
    template <class T>
    T* alloc() const {
        return (T*)alloc(alloc_info<T>()).buffer;
    }

    /// Deallocate memory allocated using this allocator.
    void dealloc(MemSlice mem) const { return vtable->dealloc(data, mem); }

    /// Reallocate a section of memory allocated using this allocator.
    MemSlice realloc(MemSlice old_mem, AllocInfo new_info) const {
        return vtable->realloc(data, old_mem, new_info);
    }
};

template <class T>
struct Allocated {
    T object;
    Allocator allocator;
};

}
}
