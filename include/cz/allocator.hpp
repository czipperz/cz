#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <new>
#include "alloc_info.hpp"
#include "slice.hpp"

namespace cz {

/// A memory allocator.
struct Allocator {
    struct VTable {
        void* (*alloc)(void* data, AllocInfo new_info);
        void (*dealloc)(void* data, MemSlice old_mem);
        void* (*realloc)(void* data, MemSlice old_mem, AllocInfo new_info);
    };

    const Allocator::VTable* vtable;
    void* data;

    /// Allocate memory using this allocator.
    void* alloc(AllocInfo info) const { return vtable->alloc(data, info); }

    /// Allocate memory to store a value of the given type using this allocator.
    template <class T>
    T* alloc() const {
        return (T*)alloc(alloc_info<T>()).buffer;
    }

    /// Allocate and initialize an object of the given type using this allocator.
    template <class T>
    T* create() const {
        T* obj = alloc<T>();
        new (obj) T();
        return obj;
    }

    /// Duplicate a slice of memory by allocating a copy of it using this allocator.
    template <class T>
    cz::Slice<T> duplicate(cz::Slice<T> slice) {
        T* new_elems = static_cast<T*>(alloc({sizeof(T) * slice.len, alignof(T)}));
        memcpy(new_elems, slice.elems, sizeof(T) * slice.len);
        return {new_elems, slice.len};
    }

    /// Deallocate memory allocated using this allocator.
    void dealloc(MemSlice mem) const { return vtable->dealloc(data, mem); }

    /// Reallocate a section of memory allocated using this allocator.
    void* realloc(MemSlice old_mem, AllocInfo new_info) const {
        return vtable->realloc(data, old_mem, new_info);
    }
};

}
