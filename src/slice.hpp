#pragma once

#include <stddef.h>

namespace cz {

/// A slice of an array of the given type.
///
/// If you want the inner type to be \c void, instead use \c MemSlice.
template <class T>
struct Slice {
    T* buffer;
    size_t len;

    constexpr T& operator[](size_t index) const { return buffer[index]; }
};

/// A slice of memory with an unspecified type.
///
/// Since \c void* cannot be indexed to because the type of the data cannot be
/// statically determined, \c MemSlice doesn't support indexing as \c Slice
/// does.
struct MemSlice {
    void* buffer;
    size_t size;
};

}
