#pragma once

namespace cz {

template <class T, class I>
struct RelPtr {
    I offset;

    void set(T* t) { offset = reinterpret_cast<const char*>(t) - reinterpret_cast<char*>(this); }

    constexpr T* get() const {
        return const_cast<T*>(
            reinterpret_cast<const T*>(reinterpret_cast<const char*>(this) + offset));
    }

    constexpr T* operator->() const { return get(); }

    constexpr T& operator*() const { return *get(); }
};

}
