#pragma once

#include "list.hpp"

namespace cz {

template <class T, size_t Capacity>
class ArrayList : public List<T> {
    static_assert(Capacity != 0, "Array must have non-zero capacity");
    alignas(T) char _buffer[sizeof(T) * Capacity];

public:
    constexpr ArrayList() : List<T>(reinterpret_cast<T*>(_buffer), 0, Capacity) {}

    ArrayList(const ArrayList& other) = default;
    ArrayList& operator=(const ArrayList& other) = default;
};

template <class T, size_t Length>
class Array {
    T _elems[sizeof(T) * Length];

public:
    constexpr Array() {}

    T* elems() { return _elems; }
    const T* elems() const { return _elems; }

    size_t len() { return Length; }

    Slice<T> as_slice() { return {elems(), Length}; }
    Slice<const T> as_slice() const { return {elems(), Length}; }

    operator Slice<T>() { return as_slice(); }
    operator Slice<const T>() const { return as_slice(); }

    Array(const Array& other) = default;
    Array& operator=(const Array& other) = default;
};

}
