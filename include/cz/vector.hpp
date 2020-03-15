#pragma once

#include "context.hpp"
#include "util.hpp"

namespace cz {

template <class T>
struct Vector {
    T* _elems;
    size_t _len;
    size_t _cap;

    void reserve(Allocator allocator, size_t extra) {
        if (this->cap() - this->len() < extra) {
            size_t new_cap = max(this->len() + extra, this->cap() * 2);

            T* new_elems;
            if (this->elems()) {
                new_elems = static_cast<T*>(allocator
                                                .realloc({this->elems(), this->cap() * sizeof(T)},
                                                         {new_cap * sizeof(T), alignof(T)})
                                                .buffer);
            } else {
                new_elems =
                    static_cast<T*>(allocator.alloc({new_cap * sizeof(T), alignof(T)}).buffer);
            }

            CZ_ASSERT(new_elems != nullptr);

            this->_elems = new_elems;
            this->_cap = new_cap;
        }
    }

    void drop(Allocator allocator) {
        if (this->elems()) {
            allocator.dealloc({this->elems(), this->cap() * sizeof(T)});
        }
    }

    void realloc(Allocator allocator) {
        if (_elems) {
            T* new_elems = static_cast<T*>(
                allocator.realloc({_elems, _cap}, {_len * sizeof(T), alignof(T)}).buffer);
            if (new_elems) {
                _elems = new_elems;
                _cap = _len;
            }
        }
    }

    Vector clone(Allocator allocator) const {
        Vector result = {};
        result.reserve(allocator, _len);
        memcpy(result._elems, _elems, _len * sizeof(T));
        result._len = _len;
        return result;
    }

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

    void remove(size_t index) {
        CZ_DEBUG_ASSERT(index < _len);
        memmove(_elems + index, _elems + index + 1, sizeof(T) * (_len - index - 1));
        --_len;
    }

    void remove_range(size_t start, size_t end) {
        CZ_DEBUG_ASSERT(start <= end);
        CZ_DEBUG_ASSERT(end <= _len);
        memmove(_elems + start, _elems + end, sizeof(T) * (_len - end));
        _len -= end - start;
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
