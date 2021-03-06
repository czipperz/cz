#pragma once

#include "allocator.hpp"
#include "assert.hpp"
#include "slice.hpp"
#include "util.hpp"

namespace cz {

template <class T>
struct Vector {
    T* _elems;
    size_t _len;
    size_t _cap;

    void reserve(Allocator allocator, size_t extra) {
        return reserve_total(allocator, _len + extra);
    }

    void reserve_total(Allocator allocator, size_t total) {
        if (this->cap() < total) {
            size_t new_cap = max(total, this->cap() * 2);

            T* new_elems = allocator.realloc(_elems, _cap, new_cap);
            CZ_ASSERT(new_elems != nullptr);

            this->_elems = new_elems;
            this->_cap = new_cap;
        }
    }

    void drop(Allocator allocator) { allocator.dealloc(_elems, _cap); }

    /// Reallocate such that the capacity matches the length.
    void realloc(Allocator allocator) {
        T* new_elems = allocator.realloc(_elems, _cap, _len);
        if (new_elems) {
            _elems = new_elems;
            _cap = _len;
        }
    }

    /// Reallocates the vector to have `new_cap` as the new capacity.  If this
    /// causes the vector to shrink then the length will be adjusted accordingly.
    void resize(Allocator allocator, size_t new_cap) {
        T* new_elems = allocator.realloc(_elems, _cap, new_cap);

        if (new_elems) {
            _elems = new_elems;
            _cap = new_cap;
            if (_cap < _len) {
                _len = _cap;
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

    void append(Slice<const T> slice) {
        CZ_DEBUG_ASSERT(_cap - _len >= slice.len);
        memcpy(_elems + _len, slice.elems, slice.len * sizeof(T));
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

    void insert_slice(size_t index, cz::Slice<const T> slice) {
        CZ_DEBUG_ASSERT(index <= _len);
        CZ_DEBUG_ASSERT(_cap - _len >= slice.len);
        memmove(_elems + index + slice.len, _elems + index, (_len - index) * sizeof(T));
        memcpy(_elems + index, slice.elems, slice.len * sizeof(T));
        _len += slice.len;
    }

    void remove(size_t index) {
        CZ_DEBUG_ASSERT(index < _len);
        memmove(_elems + index, _elems + index + 1, sizeof(T) * (_len - index - 1));
        --_len;
    }

    void remove_many(size_t index, size_t count) {
        CZ_DEBUG_ASSERT(index + count <= _len);
        memmove(_elems + index, _elems + index + count, sizeof(T) * (_len - index - count));
        _len -= count;
    }

    void remove_range(size_t start, size_t end) {
        CZ_DEBUG_ASSERT(end >= start);
        return remove_many(start, end - start);
    }

    T& last() {
        CZ_DEBUG_ASSERT(len() > 0);
        return (*this)[len() - 1];
    }
    const T& last() const {
        CZ_DEBUG_ASSERT(len() > 0);
        return (*this)[len() - 1];
    }

    T& operator[](size_t i) {
        CZ_DEBUG_ASSERT(i < len());
        return elems()[i];
    }
    const T& operator[](size_t i) const {
        CZ_DEBUG_ASSERT(i < len());
        return elems()[i];
    }

    operator Slice<T>() { return {elems(), _len}; }
    constexpr operator Slice<const T>() const { return {elems(), _len}; }

    Slice<T> as_slice() { return *this; }
    constexpr Slice<const T> as_slice() const { return *this; }

    Slice<T> slice(size_t start, size_t end) const { return {_elems + start, end - start}; }
    Slice<T> slice(const T* start, size_t end) const { return slice(start - _elems, end); }
    Slice<T> slice(size_t start, const T* end) const { return slice(start, end - _elems); }
    Slice<T> slice(const T* start, const T* end) const {
        return slice(start - _elems, end - _elems);
    }

    Slice<T> slice_start(size_t start) const { return slice(start, _len); }
    Slice<T> slice_start(const T* start) const { return slice(start, _len); }

    Slice<T> slice_end(size_t end) const { return slice((size_t)0, end); }
    Slice<T> slice_end(const T* end) const { return slice((size_t)0, end); }

    T* elems() { return _elems; }
    constexpr const T* elems() const { return _elems; }
    void set_len(size_t new_len) {
        CZ_DEBUG_ASSERT(new_len <= cap());
        _len = new_len;
    }
    constexpr size_t len() const { return _len; }
    constexpr size_t cap() const { return _cap; }

    constexpr size_t remaining() const { return  _cap - _len; }

    T* begin() { return elems(); }
    constexpr const T* begin() const { return elems(); }
    T* start() { return elems(); }
    constexpr const T* start() const { return elems(); }
    T* end() { return elems() + len(); }
    constexpr const T* end() const { return elems() + len(); }
};

template <class T>
Vector<T> Slice<T>::clone(Allocator allocator) const {
    T* ts = allocator.alloc<T>(len);
    CZ_ASSERT(ts);
    memcpy(ts, elems, sizeof(T) * len);
    return {ts, len, len};
}

}
