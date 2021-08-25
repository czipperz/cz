#pragma once

#include <stddef.h>

namespace cz {
struct Allocator;
template <class T>
struct Vector;

/// A slice of an array of the given type.
///
/// If you want the inner type to be \c void, instead use \c MemSlice.
template <class T>
struct Slice {
    T* elems;
    size_t len;

    constexpr Slice() = default;
    constexpr Slice(T* elems, size_t len) : elems(elems), len(len) {}
    template <size_t len>
    constexpr Slice(T (&arr)[len]) : elems(arr), len(len) {}

    constexpr T& operator[](size_t index) const { return elems[index]; }

    constexpr operator Slice<const T>() const { return {elems, len}; }

    // Note the implementation for this is in vector.hpp.
    Vector<T> clone(Allocator allocator) const;

    constexpr T* begin() const { return elems; }
    constexpr T* start() const { return elems; }
    constexpr T* end() const { return elems + len; }

    T& first() const { return elems[0]; }
    T& last() const { return elems[len - 1]; }

    Slice slice(size_t start, size_t end) const { return {elems + start, end - start}; }
    Slice slice(const T* start, size_t end) const { return slice(start - elems, end); }
    Slice slice(size_t start, const T* end) const { return slice(start, end - elems); }
    Slice slice(const T* start, const T* end) const { return slice(start - elems, end - elems); }

    Slice slice_start(size_t start) const { return slice(start, len); }
    Slice slice_start(const T* start) const { return slice(start, len); }

    Slice slice_end(size_t end) const { return slice((size_t)0, end); }
    Slice slice_end(const T* end) const { return slice((size_t)0, end); }
};

template <class T, size_t len_>
constexpr size_t len(T (&arr)[len_]) {
    return len_;
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
    template <size_t size2>
    constexpr MemSlice(char (&buffer)[size2]) : buffer(buffer), size(size2) {}
    template <class T>
    constexpr MemSlice(Slice<T> slice) : buffer(slice.elems), size(slice.len * sizeof(T)) {}
    constexpr MemSlice(void* buffer, size_t size) : buffer(buffer), size(size) {}

    constexpr bool operator==(MemSlice other) const {
        return buffer == other.buffer && size == other.size;
    }
    constexpr bool operator!=(MemSlice other) const { return !(*this == other); }

    constexpr void* start() const { return buffer; }
    void* end() const { return (char*)buffer + size; }

    constexpr bool contains(MemSlice other) const {
        return start() <= other.start() && end() >= other.end();
    }
};

}
