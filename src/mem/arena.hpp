#pragma once

#include <stddef.h>
#include "allocator.hpp"

namespace cz {
namespace mem {

struct Arena {
    /// Allocate a chunck of memory and make an \c cz::Arena out of it.
    static Arena sized(size_t size);

    /// Make an \c cz::Arena from the buffer and of the given size.
    Arena(void* buffer, size_t size);

    /// Create an uninitialized Arena.
    Arena();

    /// Allocate memory.  Returns null if there isn't enough space.
    void* alloc(size_t size);

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
}
