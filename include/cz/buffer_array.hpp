#pragma once

#include <stdint.h>
#include "allocator.hpp"
#include "arena.hpp"

namespace cz {

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
