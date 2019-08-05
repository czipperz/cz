#pragma once

#include "context.hpp"

namespace cz {
namespace impl {

template <class T, class Child>
class ListBase {
protected:
    size_t _len;

public:
    constexpr ListBase(size_t len) : _len(len) {}

    ListBase(const ListBase&) = delete;
    ListBase& operator=(const ListBase&) = delete;

    void push(C* c, T t) {
        _lb_reserve(c, 1);
        _lb_elems()[_len] = t;
        ++_len;
    }

    void insert(C* c, size_t index, T t) {
        CZ_ASSERT(c, index <= _len);
        _lb_reserve(c, 1);
        auto elems = _lb_elems();
        memmove(elems + index + 1, elems + index, (_len - index) * sizeof(T));
        elems[index] = t;
        ++_len;
    }

    T& operator[](size_t i) { return _lb_elems()[i]; }
    constexpr const T& operator[](size_t i) const { return _lb_elems()[i]; }

    operator Slice<T>() { return {_lb_elems(), _len}; }
    constexpr operator Slice<const T>() const { return {_lb_elems(), _len}; }

    constexpr size_t len() const { return _len; }

private:
    void _lb_reserve(C* c, size_t extra) { return static_cast<Child*>(this)->reserve(c, extra); }
    T* _lb_elems() { return static_cast<Child*>(this)->elems(); }
    const T* _lb_elems() const { return static_cast<const Child*>(this)->elems(); }
    size_t _lb_cap() { return static_cast<Child*>(this)->cap(); }
};

}
}
