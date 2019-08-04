#pragma once

#include "context.hpp"

namespace cz {
namespace impl {

template <class T, class Child>
class VectorBase {
public:
    T* elems;
    size_t len;
    size_t cap;

    constexpr VectorBase() : elems(NULL), len(0), cap(0) {}
    constexpr VectorBase(T* elems, size_t len) : elems(elems), len(len), cap(len) {}
    constexpr VectorBase(T* elems, size_t len, size_t cap) : elems(elems), len(len), cap(cap) {}

    void push(C* c, T t) {
        static_cast<Child*>(this)->reserve(c, 1);
        elems[len] = t;
        ++len;
    }

    void insert(C* c, size_t index, T t) {
        CZ_ASSERT(c, index <= len);
        static_cast<Child*>(this)->reserve(c, 1);
        memmove(elems + index + 1, elems + index, (len - index) * sizeof(T));
        elems[index] = t;
        ++len;
    }

    T& operator[](size_t i) { return elems[i]; }
    const T& operator[](size_t i) const { return elems[i]; }

    constexpr operator Slice<T>() const { return {elems, len}; }
};

}
}
