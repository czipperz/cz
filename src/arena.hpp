#pragma once

#include <stddef.h>

namespace cz {

struct Allocator;

struct Arena {
    /// Allocate a chunck of memory and make an \c cz::Arena out of it.
    static Arena sized(size_t size);

    /// Make an \c cz::Arena from the buffer and of the given size.
    Arena(void* buffer, size_t size);

    /// Create an uninitialized Arena.
    Arena();

    /// Allocate memory.  Returns null if there isn't enough space.
    void* alloc(size_t size);

    /// Allocate memory of \c new_size.  Doesn't deallocate memory until \c drop
    /// is called.
    void* realloc(void* old_ptr, size_t old_size, size_t new_size);

    /// Free the memory controlled by this \c Arena.
    ///
    /// This calls \c dealloc.
    void drop();

    /// Create an \c cz::Allocator allocating memory in the \c Arena.
    Allocator allocator();

private:
    char* start;
    char* point;
    char* end;
};

}
