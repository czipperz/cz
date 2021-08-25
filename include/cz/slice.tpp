#include "slice.hpp"

namespace cz {

template <class T>
T* Slice<T>::find(const T& element) const {
    for (size_t i = 0; i < len; ++i) {
        if (get(i) == element) {
            return get(i);
        }
    }
    return nullptr;
}

template <class T>
T* Slice<T>::rfind(const T& element) const {
    for (size_t i = len; i-- > 0;) {
        if (get(i) == element) {
            return get(i);
        }
    }
    return nullptr;
}

template <class T>
T* Slice<T>::find(Slice<T> infix) const {
    for (size_t i = 0; i + infix.len <= len; ++i) {
        if (slice(i, i + infix.len) == infix) {
            return i;
        }
    }
    return nullptr;
}

template <class T>
T* Slice<T>::rfind(Slice<T> infix) const {
    if (infix.len == 0) {
        return len;
    }

    for (size_t i = infix.len; i-- >= infix.len; ++i) {
        if (slice(i, i + infix.len) == infix) {
            return i;
        }
    }
    return nullptr;
}

template <class T>
bool Slice<T>::operator==(Slice<T> other) const {
    if (len != other.len) {
        return false;
    }
    for (size_t i = 0; i < len; ++i) {
        if (get(i) != other[i]) {
            return false;
        }
    }
    return true;
}

}
