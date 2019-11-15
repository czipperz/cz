#pragma once

#include <stddef.h>

namespace cz {

/// A slice of an array of the given type.
///
/// If you want the inner type to be \c void, instead use \c MemSlice.
template <class T>
struct Slice {
    T* elems;
    size_t len;

    constexpr T& operator[](size_t index) const { return elems[index]; }
};

template <class T>
constexpr Slice<T> slice() {
    return {};
}

template <class T, size_t len>
constexpr Slice<T> slice(T (&arr)[len]) {
    return {arr, len};
}

template <class T>
constexpr Slice<T> slice(T* elems, size_t len) {
    return {elems, len};
}

/// A slice of memory with an unspecified type.
///
/// Since \c void* cannot be indexed to because the type of the data cannot be
/// statically determined, \c MemSlice doesn't support indexing as \c Slice
/// does.
struct MemSlice {
    void* buffer;
    size_t size;

    constexpr MemSlice() : buffer(nullptr), size(0) {}
    template <size_t size>
    constexpr MemSlice(char (&buffer)[size]) : buffer(buffer), size(size) {}
    template <class T>
    constexpr MemSlice(Slice<T> slice) : buffer(slice.elems), size(slice.len * sizeof(T)) {}
    constexpr MemSlice(void* buffer, size_t size) : buffer(buffer), size(size) {}

    constexpr bool operator==(MemSlice other) const {
        return buffer == other.buffer && size == other.size;
    }
    constexpr bool operator!=(MemSlice other) const { return !(*this == other); }

    constexpr void* start() const { return buffer; }
    constexpr void* end() const { return (char*)buffer + size; }

    constexpr bool contains(MemSlice other) const {
        return start() <= other.start() && end() >= other.end();
    }
};

}
