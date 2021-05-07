#include <cz/buffer_array.hpp>

#include <stdlib.h>
#include <string.h>
#include <cz/alloc_utils.hpp>
#include <cz/assert.hpp>
#include <cz/heap.hpp>
#include <cz/util.hpp>

namespace cz {

using namespace cz;

void Buffer_Array::init() {
    num_buffers = 4;
    outer = 0;

    buffers = cz::heap_allocator().alloc_zeroed<char*>(num_buffers);
    CZ_ASSERT(buffers);

    char* buffer = cz::heap_allocator().alloc<char>(Buffer_Array::buffer_size);
    CZ_ASSERT(buffer);
    buffers[outer] = buffer;
    inner = buffer;
}

static void* buffer_array_alloc_inplace(Buffer_Array* buffer_array,
                                        AllocInfo new_info,
                                        char* start) {
    size_t size = start - buffer_array->buffers[buffer_array->outer];
    if (size >= Buffer_Array::buffer_size) {
        return nullptr;
    } else {
        return cz::advance_ptr_to_alignment({start, Buffer_Array::buffer_size - size}, new_info);
    }
}

static void* buffer_array_alloc_new_buffer(Buffer_Array* buffer_array, AllocInfo new_info) {
    // we need more space to store buffers
    if (buffer_array->outer + 1 >= buffer_array->num_buffers) {
        size_t new_size = buffer_array->num_buffers * 2;
        char** buffers = (char**)cz::heap_allocator().realloc(
            {buffer_array->buffers, sizeof(char*) * buffer_array->num_buffers},
            {sizeof(char*) * new_size, alignof(char*)});
        CZ_ASSERT(buffers);
        memset(buffers + buffer_array->num_buffers, 0,
               (new_size - buffer_array->num_buffers) * sizeof(char*));
        buffer_array->buffers = buffers;
        buffer_array->num_buffers = new_size;
        // Allocate new buffer.
    } else if (buffer_array->buffers[buffer_array->outer + 1] == nullptr) {
        // Allocate new buffer.
    } else if (new_info.size > Buffer_Array::buffer_size) {
        // Reallocate the buffer to ensure it's big enough.
        ++buffer_array->outer;
        char* buffer = (char*)cz::heap_allocator().realloc(
            {buffer_array->buffers[buffer_array->outer], Buffer_Array::buffer_size},
            {new_info.size, 1});
        CZ_ASSERT(buffer);
        buffer_array->buffers[buffer_array->outer] = buffer;
        return buffer;
    } else {
        // We've already allocated a buffer.
        ++buffer_array->outer;
        return buffer_array->buffers[buffer_array->outer];
    }

    // Make another buffer.
    size_t buffer_size = cz::max<size_t>(Buffer_Array::buffer_size, new_info.size);
    char* buffer = cz::heap_allocator().alloc<char>(buffer_size);
    CZ_ASSERT(buffer);
    ++buffer_array->outer;
    buffer_array->buffers[buffer_array->outer] = buffer;

    return buffer;
}

static void* buffer_array_alloc(void* data, AllocInfo new_info) {
    Buffer_Array* buffer_array = static_cast<Buffer_Array*>(data);

    void* ptr = buffer_array_alloc_inplace(buffer_array, new_info, buffer_array->inner);
    if (!ptr) {
        ptr = buffer_array_alloc_new_buffer(buffer_array, new_info);
    }

    buffer_array->inner = static_cast<char*>(ptr) + new_info.size;

    return ptr;
}

static void buffer_array_dealloc(void* data, MemSlice old_mem) {
    Buffer_Array* buffer_array = static_cast<Buffer_Array*>(data);
    // We only dealloc the last entry.
    if (buffer_array->inner == static_cast<char*>(old_mem.buffer) + old_mem.size) {
        // Likely case
    } else {
        CZ_DEBUG_ASSERT(buffer_array->inner == buffer_array->buffers[buffer_array->outer]);
        CZ_DEBUG_ASSERT(buffer_array->outer > 0);
        CZ_DEBUG_ASSERT(old_mem.buffer >= buffer_array->buffers[buffer_array->outer - 1]);
        CZ_DEBUG_ASSERT(old_mem.buffer <
                        buffer_array->buffers[buffer_array->outer - 1] + Buffer_Array::buffer_size);

        --buffer_array->outer;
    }

    buffer_array->inner = static_cast<char*>(old_mem.buffer);
}

static void* buffer_array_realloc(void* data, MemSlice old_mem, AllocInfo new_info) {
    Buffer_Array* buffer_array = static_cast<Buffer_Array*>(data);
    // we only realloc the last entry
    CZ_DEBUG_ASSERT(buffer_array->inner == static_cast<char*>(old_mem.buffer) + old_mem.size);

    void* ptr =
        buffer_array_alloc_inplace(buffer_array, new_info, static_cast<char*>(old_mem.buffer));
    if (!ptr) {
        ptr = buffer_array_alloc_new_buffer(buffer_array, new_info);
        memcpy(ptr, old_mem.buffer, cz::min<size_t>(old_mem.size, new_info.size));
    }

    buffer_array->inner = static_cast<char*>(ptr) + new_info.size;

    return ptr;
}

Allocator Buffer_Array::allocator() {
    static const Allocator::VTable vtable = {
        buffer_array_alloc,
        buffer_array_dealloc,
        buffer_array_realloc,
    };
    return {&vtable, this};
}

void Buffer_Array::drop() {
    for (size_t i = 0; i < num_buffers; ++i) {
        cz::heap_allocator().dealloc({buffers[i], Buffer_Array::buffer_size});
    }
    cz::heap_allocator().dealloc(buffers, num_buffers);
}

}
