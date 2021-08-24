#pragma once

#include <stdint.h>
#include "allocator.hpp"

namespace cz {

/// `Buffer_Array`: a fast slab based allocator.  Stores an array
/// of `4096`-byte buffers that elements can be allocated into.
///
/// Doesn't support deallocating anything except for the most recently allocated
/// object.  Instead, `save` the state before the allocations and `restore` after.
///
/// If deallocation fails in debug mode (because you tried to deallocate in a different
/// order than the reverse order of allocation) then the `Buffer_Array` will panic.
/// This makes it easy to debug code that is incorrectly using the `Buffer_Array`.
///
/// # Example
///
/// Reading each line from a file into a `Vector<Str>`.  Allocate the `Vector` on
/// the heap and allocate each line in a buffer array.  You cannot put the `Vector`
/// in the `Buffer_Array` because when it resizes it will attempt to deallocate
/// memory that is not the most recently allocated element of the buffer array.
///
/// For more complex layouts remember that you can allocate data temporarily on the heap
/// then transfer it into the `Buffer_Array` via the `clone` method and a deferred drop.
struct Buffer_Array {
    static constexpr const size_t buffer_size = 0x1000;

    char** buffers;
    size_t num_buffers;

    size_t buffer_index;
    char* buffer_pointer;
    char* buffer_end;

    /// Allocate the initial data.
    void init();
    /// Drop all data.
    void drop();

    /// Note: you can only reallocate / deallocate the last allocated item!
    Allocator allocator() { return {Buffer_Array::realloc, Buffer_Array::dealloc, this}; }

    /// Completely cleares all state.
    void clear() { restore({0, 0}); }

    const void* end_of_last_allocation() const { return buffer_pointer; }

    /// A save point allows you to instantly deallocate all allocations
    /// made after `save` is called just by calling `restore`.
    struct Save_Point {
        size_t outer;
        size_t inner;
    };
    inline Save_Point save() const;
    void restore(Save_Point sp);

    static void* realloc(void* buffer_array, MemSlice old_mem, AllocInfo new_info);
    static void dealloc(void* buffer_array, MemSlice old_mem);
};

inline Save_Point Buffer_Array::save() const {
    return {
        buffer_index,
        (size_t)(buffer_pointer - buffers[buffer_index]),
    };
}

}
