#pragma once

#include <cz/slice.hpp>

namespace cz {

/// Search for an element in the slice.  If an equivalent element is found then
/// stores its index in `*index` and returns `true`.  Otherwise, stores in `*index`
/// where it would reside if inserted to keep the slice in order and returns `false`.
template <class T>
bool binary_search(cz::Slice<T> slice, const T& element, size_t* index) {
    if (slice.len == 0) {
        *index = 0;
        return false;
    }

    size_t start = 0;
    size_t end = slice.len;
    while (start + 1 < end) {
        size_t mid = (start + end) / 2;
        if (element < slice[mid]) {
            end = mid;
        } else {
            start = mid;
        }
    }

    if (slice[start] < element) {
        ++start;
    }

    *index = start;
    return start < end && slice[start] == element;
}

/// Search for an element in the slice.  If an equivalent element is found then
/// stores its index in `*index` and returns `true`.  Otherwise, stores in `*index`
/// where it would reside if inserted to keep the slice in order and returns `false`.
///
/// Comparator should be of type `int64_t (*)(const T&, const T&)`.
template <class T, class Comparator>
bool binary_search(cz::Slice<T> slice, const T& element, size_t* index, Comparator&& comparator) {
    if (slice.len == 0) {
        *index = 0;
        return false;
    }

    size_t start = 0;
    size_t end = slice.len;
    while (start + 1 < end) {
        size_t mid = (start + end) / 2;
        if (comparator(element, slice[mid]) < 0) {
            end = mid;
        } else {
            start = mid;
        }
    }

    if (comparator(slice[start], element) < 0) {
        ++start;
    }

    *index = start;
    return start < end && comparator(slice[start], element) == 0;
}

}
