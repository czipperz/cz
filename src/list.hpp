#pragma once

#include "assert.hpp"
#include "context.hpp"

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
    void push(C* c, T t) {
        if (_cap - _len < 1) {
            CZ_PANIC(c, "List::push(): length exceeded");
        }
        _elems[_len] = t;
        ++_len;
    }

    T pop(C* c) {
        if (_len < 1) {
            CZ_PANIC(c, "List::pop(): No element to pop");
        }
        --_len;
        return _elems[_len];
    }

    void insert(C* c, size_t index, T t) {
        CZ_ASSERT(c, index <= _len);
        if (_cap - _len < 1) {
            CZ_PANIC(c, "List::insert(): length exceeded");
        }
        memmove(_elems + index + 1, _elems + index, (_len - index) * sizeof(T));
        _elems[index] = t;
        ++_len;
    }

    T& operator[](size_t i) { return elems()[i]; }
    constexpr const T& operator[](size_t i) const { return elems()[i]; }

    operator Slice<T>() { return {elems(), _len}; }
    constexpr operator Slice<const T>() const { return {elems(), _len}; }

    T* elems() { return _elems; }
    constexpr const T* elems() const { return _elems; }
    constexpr size_t len() const { return _len; }
    constexpr size_t cap() const { return _cap; }
};

}
