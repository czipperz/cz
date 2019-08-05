#pragma once

#include "assert.hpp"
#include "context.hpp"
#include "list_base.hpp"

namespace cz {

template <class T>
class ArrayImpl : public impl::ListBase<T, ArrayImpl<T>> {
protected:
    T* _elems;
    size_t _cap;

public:
    template <size_t cap>
    constexpr explicit ArrayImpl(T (&elems)[cap], size_t len) : ArrayImpl(elems, len, cap) {}
    constexpr explicit ArrayImpl(T* elems, size_t len, size_t cap)
        : impl::ListBase<T, ArrayImpl<T>>(len), _elems(elems), _cap(cap) {}

    void reserve(C* c, size_t extra) {
        if (_cap - this->len() < extra) {
            CZ_PANIC(c, "Array length exceeded");
        }
    }

    ArrayImpl(const ArrayImpl&) = delete;
    ArrayImpl& operator=(const ArrayImpl&) = delete;

    T* elems() { return _elems; }
    const T* elems() const { return _elems; }
    size_t cap() const { return _cap; }
};

template <class T, size_t Capacity>
class Array : public ArrayImpl<T> {
    alignas(T) char _buffer[sizeof(T) * Capacity];

public:
    constexpr Array() : ArrayImpl<T>(reinterpret_cast<T*>(_buffer), 0, Capacity) {}

    Array(const Array& other) : Array() { *this = other; }
    Array& operator=(const Array& other) {
        memcpy(_buffer, other._buffer, other.len * sizeof(T));
        this->len = other.len;
        return *this;
    }
};

template <class T>
class ArrayRef : public ArrayImpl<T> {
public:
    constexpr ArrayRef() : ArrayImpl<T>(NULL, 0, 0) {}
    template <size_t cap>
    constexpr ArrayRef(T (&elems)[cap], size_t len) : ArrayImpl<T>(elems, len, cap) {}
    constexpr ArrayRef(T* elems, size_t len, size_t cap) : ArrayImpl<T>(elems, len, cap) {}

    constexpr ArrayRef(ArrayRef&& other) : ArrayRef(other.elems(), other.len(), other.cap()) {}
    ArrayRef& operator=(ArrayRef&& other) {
        this->_elems = other.elems();
        this->_len = other.len();
        this->_cap = other.cap();
        return *this;
    }
};

}
