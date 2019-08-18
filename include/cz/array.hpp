#pragma once

#include "list.hpp"

namespace cz {

template <class T, size_t Capacity>
class Array : public List<T> {
    static_assert(Capacity != 0, "Array must have non-zero capacity");
    alignas(T) char _buffer[sizeof(T) * Capacity];

public:
    constexpr Array() : List<T>(reinterpret_cast<T*>(_buffer), 0, Capacity) {}

    Array(const Array& other) = default;
    Array& operator=(const Array& other) = default;
};

}
