#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <new>
#include "alloc_info.hpp"
#include "assert.hpp"
#include "slice.hpp"

namespace cz {

/// A memory allocator.
struct Allocator {
    void* (*func)(void* data, MemSlice old_mem, AllocInfo new_info);
    void* data;

    /// Allocate memory using this allocator.
    void* alloc(AllocInfo info) const {
        CZ_DEBUG_ASSERT(info.size > 0);
        return realloc({}, info);
    }
    /// Deallocate memory allocated using this allocator.
    void dealloc(MemSlice old_mem) const { realloc(old_mem, {0, 1}); }

#ifndef NDEBUG
    // When compiled in debug mode we have out of line handlers that check preconditions
    // and fill uninitialized memory with random values to try to find bugs.
    /// Reallocate memory allocated using this allocator.
    void* realloc(MemSlice old_mem, AllocInfo new_info) const;
#else
    // When compiled in release mode call the virtual function without any checks.
    /// Reallocate memory allocated using this allocator.
    void* realloc(MemSlice old_mem, AllocInfo new_info) const {
        return func(data, old_mem, new_info);
    }
#endif

    /// Allocate memory to store a value of the given type using this allocator.
    template <class T>
    T* alloc() const {
        return (T*)alloc(alloc_info<T>());
    }

    /// Allocate memory to store an array of values of the given type using this allocator.
    template <class T>
    T* alloc(size_t count) const {
        AllocInfo info = alloc_info<T>();
        info.size *= count;
        return (T*)alloc(info);
    }

    /// Allocate zeroed memory to store an array of values of the given type using this allocator.
    template <class T>
    T* alloc_zeroed(size_t count) const {
        AllocInfo info = alloc_info<T>();
        info.size *= count;
        T* ptr = (T*)alloc(info);
        if (ptr) {
            memset(ptr, 0, info.size);
        }
        return ptr;
    }

    /// Allocate and initialize an object of the given type using this allocator.
    template <class T>
    T* create() const {
        T* obj = alloc<T>();
        CZ_ASSERT(obj);
        new (obj) T();
        return obj;
    }

    /// Clone an object into the allocator.
    template <class T>
    T* clone(const T& t) {
        T* ptr = alloc<T>();
        CZ_ASSERT(ptr);
        *ptr = t;
        return ptr;
    }

    /// Duplicate a slice of memory by allocating a copy of it using this allocator.
    template <class T>
    cz::Slice<T> duplicate(cz::Slice<T> slice) {
        T* new_elems = static_cast<T*>(alloc({sizeof(T) * slice.len, alignof(T)}));
        CZ_ASSERT(new_elems);
        memcpy(new_elems, slice.elems, sizeof(T) * slice.len);
        return {new_elems, slice.len};
    }

    /// Deallocate an array of a given type or a single element if the count isn't passed.
    template <class T>
    void dealloc(T* t, size_t count = 1) const {
        return dealloc({t, sizeof(T) * count});
    }

    /// Reallocate an array of a given type.
    template <class T>
    T* realloc(T* old_mem, size_t old_len, size_t new_len) const {
        return (T*)realloc({old_mem, old_len * sizeof(T)}, {new_len * sizeof(T), alignof(T)});
    }
};

}
