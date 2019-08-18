#pragma once

#include "list.hpp"

namespace cz {

template <class T>
class ArrayRef : public List<T> {
public:
    constexpr ArrayRef() : ArrayRef(nullptr, 0, 0) {}
    template <size_t cap>
    constexpr ArrayRef(T (&elems)[cap], size_t len) : ArrayRef(elems, len, cap) {}
    constexpr ArrayRef(T* elems, size_t len, size_t cap) : List<T>(elems, len, cap) {}

    constexpr ArrayRef(ArrayRef&& other) = default;
    ArrayRef& operator=(ArrayRef&& other) = default;
};

}
