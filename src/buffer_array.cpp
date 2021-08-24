#include <cz/buffer_array.hpp>

#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <cz/alloc_utils.hpp>
#include <cz/assert.hpp>
#include <cz/heap.hpp>
#include <cz/util.hpp>

namespace cz {

constexpr const size_t Buffer_Array::buffer_size;

void Buffer_Array::init() {
    num_buffers = 4;

    buffers = cz::heap_allocator().alloc_zeroed<char*>(num_buffers);
    CZ_ASSERT(buffers);

    char* buffer = cz::heap_allocator().alloc<char>(Buffer_Array::buffer_size);
    CZ_ASSERT(buffer);
    buffers[0] = buffer;

    buffer_index = 0;
    buffer_pointer = buffer;
    buffer_end = buffer + Buffer_Array::buffer_size;
}

void Buffer_Array::drop() {
    for (size_t i = 0; i < num_buffers; ++i) {
        cz::heap_allocator().dealloc({buffers[i], Buffer_Array::buffer_size});
    }
    cz::heap_allocator().dealloc(buffers, num_buffers);
}

static char* get_starting_point(Buffer_Array* buffer_array, MemSlice old_mem) {
    char* starting_point = (char*)old_mem.buffer;

    // If we don't match exactly then we must be deallocating from the previous buffer.
    if (old_mem.end() != buffer_array->buffer_pointer) {
#ifndef NDEBUG
        {
            // Note to the reader: deallocation from a `Buffer_Array` will panic if it
            // is not the most recently allocated object.  We do this to catch "hole"s
            // in the buffer array.  If you really want to use a buffer array then you
            // will need to use multiple.  Adding a buffer array that tracks the stack
            // (scoped allocation/deallocation) will typically do the trick.

            // Assert we're not skipping any allocations in this buffer.
            if (buffer_array->buffer_pointer != buffer_array->buffers[buffer_array->buffer_index]) {
                CZ_PANIC("Deallocation would skip allocated items");
            }

            // Assert we have a previous buffer.
            if (buffer_array->buffer_index == 0) {
                CZ_PANIC("Dealloc called with invalid pointer -- first buffer");
            }

            // Assert we are in bounds of the previous buffer.
            char* buffer_start = buffer_array->buffers[buffer_array->buffer_index - 1];
            if (old_mem.buffer < buffer_start ||
                old_mem.buffer >= buffer_start + Buffer_Array::buffer_size) {
                CZ_PANIC("Dealloc called with invalid pointer");
            }
        }
#endif

        char* buffer_start = buffer_array->buffers[buffer_array->buffer_index - 1];

        // Retreat to the previous buffer.
        --buffer_array->buffer_index;
        buffer_array->buffer_pointer = (char*)old_mem.end();

        // Calculate the buffer end.
        if (old_mem.size > Buffer_Array::buffer_size) {
            // We have a massive allocation that has its own buffer.
            CZ_DEBUG_ASSERT(old_mem.buffer == buffer_start);
            buffer_array->buffer_end = buffer_start + old_mem.size;
        } else {
            buffer_array->buffer_end = buffer_start + Buffer_Array::buffer_size;
        }
    }

    return starting_point;
}

void Buffer_Array::dealloc(void* _buffer_array, MemSlice old_mem) {
    // Nothing to be done here.
    if (!old_mem.buffer) {
        return;
    }

    Buffer_Array* buffer_array = (Buffer_Array*)_buffer_array;
    buffer_array->buffer_pointer = get_starting_point(buffer_array, old_mem);
}

void* Buffer_Array::realloc(void* _buffer_array, MemSlice old_mem, AllocInfo new_info) {
    Buffer_Array* buffer_array = (Buffer_Array*)_buffer_array;

    // First get the available memory to allocate.
    char* starting_point;
    if (old_mem.buffer) {
        // We can reuse the memory for the buffer because it was
        // the most recently allocated object in this buffer.
        starting_point = get_starting_point(buffer_array, old_mem);
    } else {
        // We can use the memory left in the buffer.
        starting_point = buffer_array->buffer_pointer;
    }

    // If this condition is false then the advance call will be total bogus.
    CZ_DEBUG_ASSERT(buffer_array->buffer_end >= starting_point);

    // If there is enough space in this buffer then we allocate in it.
    void* ptr = advance_ptr_to_alignment(
        {starting_point, (size_t)(buffer_array->buffer_end - starting_point)}, new_info);
    if (ptr) {
        // No need to memcpy because we either are expanding
        // the existing allocation or creating a new one.
        buffer_array->buffer_pointer = (char*)ptr + new_info.size;
        return ptr;
    }

    // Expand the outer array in preparation for adding the new buffer.
    if (buffer_array->buffer_index + 1 == buffer_array->num_buffers) {
        char** new_buffers = cz::heap_allocator().realloc(
            buffer_array->buffers, buffer_array->num_buffers, buffer_array->num_buffers * 2);
        CZ_ASSERT(new_buffers);
        memset(new_buffers + buffer_array->num_buffers, 0,
               buffer_array->num_buffers * sizeof(*buffer_array->buffers));
        buffer_array->buffers = new_buffers;
        buffer_array->num_buffers *= 2;
    }

    // Allocate the new buffer.
    size_t this_buffer_size = std::max(Buffer_Array::buffer_size, new_info.size);
    char* buffer = (char*)cz::heap_allocator().alloc({this_buffer_size, new_info.alignment});
    CZ_ASSERT(buffer);

    // Add it to the array.
    ++buffer_array->buffer_index;
    buffer_array->buffers[buffer_array->buffer_index] = buffer;
    buffer_array->buffer_pointer = buffer + new_info.size;
    buffer_array->buffer_end = buffer + this_buffer_size;

    // Copy over the old contents if applicable.
    if (old_mem.buffer) {
        // We have to be expanding because otherwise we would've shrunk in place.
        CZ_DEBUG_ASSERT(old_mem.size < new_info.size);
        memcpy(buffer, old_mem.buffer, old_mem.size);
    }

    return buffer;
}

void Buffer_Array::restore(Save_Point sp) {
#ifndef NDEBUG
    const unsigned char dealloc_fill = 0xDD;

    // Fill each buffer that is completely deallocated.
    for (size_t i = sp.outer + 1; i <= buffer_index; ++i) {
        memset(buffers[i], dealloc_fill, Buffer_Array::buffer_size);
    }

    // Fill the rest of this buffer.
    if (sp.inner < Buffer_Array::buffer_size) {
        memset(buffers[sp.outer] + sp.inner, dealloc_fill, Buffer_Array::buffer_size - sp.inner);
    }
#endif

    buffer_index = sp.outer;
    buffer_pointer = buffers[sp.outer] + sp.inner;
    buffer_end = buffers[sp.outer] + Buffer_Array::buffer_size;

    // If we are restoring to the point after a large
    // allocation then the size is larger than buffer_size.
    if (buffer_pointer > buffer_end) {
        buffer_end = buffer_pointer;
    }
}

}
