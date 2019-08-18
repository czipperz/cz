#pragma once

namespace cz {

template <class T, class I>
struct RelPtr {
    I offset;

    constexpr T* get() const {
        return const_cast<T*>(
            reinterpret_cast<const T*>(reinterpret_cast<const char*>(this) + offset));
    }

    constexpr T* operator->() const { return get(); }

    constexpr T& operator*() const { return *get(); }
};

}
