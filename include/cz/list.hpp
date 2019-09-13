#pragma once

#include <stddef.h>
#include <string.h>
#include "assert.hpp"
#include "slice.hpp"

namespace cz {

template <class T>
class List {
protected:
    T* _elems;
    size_t _len;
    size_t _cap;

    constexpr List(T* elems, size_t len, size_t cap) : _elems(elems), _len(len), _cap(cap) {}
    List(const List&) = default;
    List& operator=(const List&) = default;

public:
    void push(T t) {
        CZ_DEBUG_ASSERT(_cap - _len >= 1);
        _elems[_len] = t;
        ++_len;
    }

    void append(Slice<T> slice) {
        CZ_DEBUG_ASSERT(_cap - _len >= slice.len);
        memcpy(_elems, slice.elems, slice.len * sizeof(T));
        _len += slice.len;
    }

    T pop() {
        CZ_DEBUG_ASSERT(_len >= 1);
        --_len;
        return _elems[_len];
    }

    void insert(size_t index, T t) {
        CZ_DEBUG_ASSERT(index <= _len);
        CZ_DEBUG_ASSERT(_cap - _len >= 1);
        memmove(_elems + index + 1, _elems + index, (_len - index) * sizeof(T));
        _elems[index] = t;
        ++_len;
    }

    T& last() {
        CZ_DEBUG_ASSERT(len() > 0);
        return (*this)[len() - 1];
    }
    constexpr const T& last() const {
        CZ_DEBUG_ASSERT(len() > 0);
        return (*this)[len() - 1];
    }

    T& operator[](size_t i) {
        CZ_DEBUG_ASSERT(i < len());
        return elems()[i];
    }
    constexpr const T& operator[](size_t i) const {
        CZ_DEBUG_ASSERT(i < len());
        return elems()[i];
    }

    operator Slice<T>() { return {elems(), _len}; }
    constexpr operator Slice<const T>() const { return {elems(), _len}; }

    Slice<T> as_slice() { return *this; }
    constexpr Slice<const T> as_slice() const { return *this; }

    T* elems() { return _elems; }
    constexpr const T* elems() const { return _elems; }
    void set_len(size_t new_len) {
        CZ_DEBUG_ASSERT(new_len <= cap());
        _len = new_len;
    }
    constexpr size_t len() const { return _len; }
    constexpr size_t cap() const { return _cap; }

    T* start() { return elems(); }
    constexpr const T* start() const { return elems(); }
    T* end() { return elems() + len(); }
    constexpr const T* end() const { return elems() + len(); }
};

}
