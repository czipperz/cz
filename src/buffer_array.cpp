#include <cz/buffer_array.hpp>

#include <stdlib.h>
#include <string.h>
#include <cz/alloc_utils.hpp>
#include <cz/assert.hpp>
#include <cz/util.hpp>

namespace cz {

using namespace cz;

void BufferArray::create() {
    num_buffers = 4;
    outer = 0;

    buffers = static_cast<char**>(calloc(sizeof(char*), num_buffers));
    CZ_ASSERT(buffers);

    char* buffer = static_cast<char*>(malloc(BufferArray::buffer_size));
    CZ_ASSERT(buffer);
    buffers[outer] = buffer;
    inner = buffer;
}

void BufferArray::clear() {
    outer = 0;
    inner = buffers[outer];
}

static void* buffer_array_alloc_inplace(BufferArray* buffer_array,
                                        AllocInfo new_info,
                                        char* start) {
    size_t size = start - buffer_array->buffers[buffer_array->outer];
    if (size >= BufferArray::buffer_size) {
        return nullptr;
    } else {
        return cz::advance_ptr_to_alignment({start, BufferArray::buffer_size - size}, new_info);
    }
}

static void* buffer_array_alloc_new_buffer(BufferArray* buffer_array, AllocInfo new_info) {
    // we need more space to store buffers
    if (buffer_array->outer + 1 >= buffer_array->num_buffers) {
        size_t new_size = buffer_array->num_buffers * 2;
        char** buffers =
            static_cast<char**>(realloc(buffer_array->buffers, sizeof(char*) * new_size));
        CZ_ASSERT(buffers);
        memset(buffers + buffer_array->num_buffers, 0,
               (new_size - buffer_array->num_buffers) * sizeof(char*));
        buffer_array->buffers = buffers;
        buffer_array->num_buffers = new_size;
        // Allocate new buffer.
    } else if (buffer_array->buffers[buffer_array->outer + 1] == nullptr) {
        // Allocate new buffer.
    } else if (new_info.size > BufferArray::buffer_size) {
        // Reallocate the buffer to ensure it's big enough.
        ++buffer_array->outer;
        char* buffer =
            static_cast<char*>(realloc(buffer_array->buffers[buffer_array->outer], new_info.size));
        CZ_ASSERT(buffer);
        buffer_array->buffers[buffer_array->outer] = buffer;
        return buffer;
    } else {
        // We've already allocated a buffer.
        ++buffer_array->outer;
        return buffer_array->buffers[buffer_array->outer];
    }

    // Make another buffer.
    size_t buffer_size = cz::max<size_t>(BufferArray::buffer_size, new_info.size);
    char* buffer = static_cast<char*>(malloc(buffer_size));
    CZ_ASSERT(buffer);
    ++buffer_array->outer;
    buffer_array->buffers[buffer_array->outer] = buffer;

    return buffer;
}

static MemSlice buffer_array_alloc(void* data, AllocInfo new_info) {
    BufferArray* buffer_array = static_cast<BufferArray*>(data);

    void* ptr = buffer_array_alloc_inplace(buffer_array, new_info, buffer_array->inner);
    if (!ptr) {
        ptr = buffer_array_alloc_new_buffer(buffer_array, new_info);
    }

    buffer_array->inner = static_cast<char*>(ptr) + new_info.size;

    return {ptr, new_info.size};
}

static void buffer_array_dealloc(void* data, MemSlice old_mem) {
    BufferArray* buffer_array = static_cast<BufferArray*>(data);
    // we only dealloc the last entry
    CZ_DEBUG_ASSERT(buffer_array->inner == static_cast<char*>(old_mem.buffer) + old_mem.size);
    buffer_array->inner = static_cast<char*>(old_mem.buffer);
}

static MemSlice buffer_array_realloc(void* data, MemSlice old_mem, AllocInfo new_info) {
    BufferArray* buffer_array = static_cast<BufferArray*>(data);
    // we only realloc the last entry
    CZ_DEBUG_ASSERT(buffer_array->inner == static_cast<char*>(old_mem.buffer) + old_mem.size);

    void* ptr =
        buffer_array_alloc_inplace(buffer_array, new_info, static_cast<char*>(old_mem.buffer));
    if (!ptr) {
        ptr = buffer_array_alloc_new_buffer(buffer_array, new_info);
        memcpy(ptr, old_mem.buffer, cz::min<size_t>(old_mem.size, new_info.size));
    }

    buffer_array->inner = static_cast<char*>(ptr) + new_info.size;

    return {ptr, new_info.size};
}

Allocator BufferArray::allocator() {
    static const Allocator::VTable vtable = {
        buffer_array_alloc,
        buffer_array_dealloc,
        buffer_array_realloc,
    };
    return {&vtable, this};
}

void BufferArray::drop() {
    for (size_t i = 0; i < num_buffers; ++i) {
        free(buffers[i]);
    }
    free(buffers);
}

}
