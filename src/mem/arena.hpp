#pragma once

#include <stddef.h>
#include "allocator.hpp"

namespace cz {
namespace mem {

struct Arena {
    MemSlice mem;
    size_t offset = 0;

    Arena() = default;
    constexpr explicit Arena(MemSlice mem) : mem(mem) {}

    /// Free the memory controlled by this \c Arena.
    ///
    /// This calls \c dealloc.
    void drop();

    /// Create an \c cz::Allocator allocating memory in the \c Arena.
    Allocator allocator();
};

}
}
