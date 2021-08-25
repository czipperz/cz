#pragma once

#include <stdint.h>
#include <string.h>
#include <cz/str.hpp>
#include <cz/util.hpp>

namespace cz {

template <class T>
inline int64_t compare(const T& left, const T& right) {
    if (left < right) {
        return -1;
    } else if (left == right) {
        return 0;
    } else {
        return 1;
    }
}
inline int64_t compare(Str left, Str right);

inline int64_t compare_same_length(const char* left, const char* right, size_t len) {
    return memcmp(left, right, len);
}
template <class T>
int64_t compare_same_length(T* left, T* right, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        int64_t comparison = compare(left[i], right[i]);
        if (comparison != 0) {
            return comparison;
        }
    }
    return 0;
}

/// Objects are sorted by length.  Then they are sorted by value.
/// Ex. `ab < de < abc`.
struct Shortlex {
    template <class T>
    int64_t compare(Slice<T> left, Slice<T> right) {
        if (left.len != right.len)
            return (int64_t)(left.len - right.len);
        return compare_same_length(left.elems, right.elems, left.len);
    }
    int64_t compare(Str left, Str right) {
        if (left.len != right.len)
            return (int64_t)(left.len - right.len);
        return compare_same_length(left.buffer, right.buffer, left.len);
    }

    /// Easy plug into algorithms that usually expect `operator<` behavior.
    template <class T>
    bool operator()(Slice<T> left, Slice<T> right) {
        return compare(left, right) < 0;
    }
    bool operator()(Str left, Str right) { return compare(left, right) < 0; }
};

/// Sort by prefix.  Resolve ties by comparing length.
/// Ex. `ab < abc < de`.
struct Lexico {
    template <class T>
    int64_t compare(Slice<T> left, Slice<T> right) {
        int64_t comparison =
            compare_same_length(left.elems, right.elems, cz::min(left.len, right.len));
        if (comparison != 0) {
            return comparison;
        }
        return (int64_t)(left.len - right.len);
    }
    int64_t compare(Str left, Str right) {
        int64_t comparison =
            compare_same_length(left.buffer, right.buffer, cz::min(left.len, right.len));
        if (comparison != 0) {
            return comparison;
        }
        return (int64_t)(left.len - right.len);
    }

    /// Easy plug into algorithms that usually expect `operator<` behavior.
    template <class T>
    bool operator()(Slice<T> left, Slice<T> right) {
        return compare(left, right) < 0;
    }
    bool operator()(Str left, Str right) { return compare(left, right) < 0; }
};

inline int64_t compare(Str left, Str right) {
    return Lexico{}.compare(left, right);
}

}
