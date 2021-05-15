#pragma once

#include <stdint.h>
#include "allocator.hpp"
#include "arena.hpp"

namespace cz {

/// A `Buffer_Array` is a super fast allocator that encourages an "allocate and forget"
/// strategy.  This is useful for when you have to store a lot of data and don't have to
/// ever selectively remove items.  For example, if you are parsing a file into a tree then
/// you can create a `Buffer_Array` to store the tree in.  A `Buffer_Array` is also optimal
/// for temporary allocations such as storing the results of `sprintf` temporarily.
///
/// A `Buffer_Array` has the following advantages over the `heap_allocator`:
///   * Allocation is way faster.
///   * Allocations are all in the same page making memory accesses much faster.
///   * Much lower overall memory usage because there is no overhead per allocation.
///   * Memory is recycled when the `Buffer_Array` is cleared.
/// And the following disadvantages:
///   * Deallocation must either be done in bulk or in the reverse order of allocation.
///   * Really large allocations (larger than `0x1000 = 4096` bytes)
///     will cause the rest of the previous page to be wasted.
///
/// Under the hood, a `Buffer_Array` is a dynamically sized array of buffers.  Each
/// buffer is `0x1000 = 4096` bytes large.  When you allocate memory the `Buffer_Array`
/// will just bump the pointer into the buffer by the corresponding amount.  If there
/// isn't enough memory in the current buffer another will be allocated.
///
/// If you try to allocate an object greater than `0x1000
/// = 4096` bytes then it will get its own buffer.
///
/// # Deallocation
///
/// A `Buffer_Array` can be simply `clear`ed or `restore`d
/// instead of deallocating each allocation individually.
///
/// Deallocation isn't 100% foolproof because of the 0 overhead approach.  Deallocating
/// the most recently allocated entry will always succeed.  But deallocating after that
/// point will only suceed if all allocations in the buffer have the same alignment.
///
/// If deallocation fails in debug mode (because you tried to deallocate in a different
/// order than the reverse order of allocation) then the `Buffer_Array` will panic.
/// This makes it easy to debug code that is incorrectly using the `Buffer_Array`.
struct Buffer_Array {
    static constexpr const size_t buffer_size = 0x1000;

    char** buffers;
    size_t num_buffers;

    size_t buffer_index;
    char* buffer_pointer;
    char* buffer_end;

    void init();
    void drop();

    Allocator allocator() { return {Buffer_Array::realloc, this}; }

    void clear() { restore({0, 0}); }

    const void* end_of_last_allocation() const { return buffer_pointer; }

    /// A save point allows you to instantly deallocate all allocations
    /// made after the point is created just by calling `restore`.
    struct Save_Point {
        size_t outer;
        size_t inner;
    };
    Save_Point save() const {
        return {
            buffer_index,
            (size_t)(buffer_pointer - buffers[buffer_index]),
        };
    }
    void restore(Save_Point sp) {
        buffer_index = sp.outer;
        buffer_pointer = buffers[sp.outer] + sp.inner;
        buffer_end = buffers[sp.outer] + Buffer_Array::buffer_size;

        // If we are restoring to the point after a large
        // allocation then the size is larger than buffer_size.
        if (buffer_pointer > buffer_end) {
            buffer_end = buffer_pointer;
        }
    }

    static void* realloc(void* buffer_array, MemSlice old_mem, AllocInfo new_info);
};

}
