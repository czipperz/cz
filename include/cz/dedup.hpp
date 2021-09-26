#pragma once

#include <cz/slice.hpp>
#include <cz/template_generic.hpp>
#include <cz/vector.hpp>

namespace cz {

/// Remove successive duplicate elements.  If the input is
/// sorted then removes all duplicates.  Returns the new end.
///
/// Requires `Iterator` to be a `std::Forward_Iterator`.
template <class Iterator, class Is_Equal, class Set>
Iterator dedup(Iterator start, Iterator end, Is_Equal&& is_equal, Set&& set) {
    Iterator insert = start;

    Iterator next = start;
    ++next;

    for (; next < end; start = next, ++next) {
        if (!is_equal(start, next)) {
            set(insert, start);
            ++insert;
        }
    }
    if (start < end) {
        set(insert, start);
        ++insert;
    }

    return insert;
}

template <class T, class Is_Equal, class Set>
cz::Slice<T> dedup(cz::Slice<T> slice, Is_Equal&& is_equal, Set&& set) {
    T* new_end = dedup(slice.start(), slice.end(), is_equal, set);
    return slice.slice_end(new_end);
}
template <class T, class Is_Equal>
cz::Slice<T> dedup(cz::Slice<T> slice, Is_Equal&& is_equal) {
    return dedup(slice.start(), slice.end(), is_equal, generic_set_ptr<T>);
}
template <class T>
cz::Slice<T> dedup(cz::Slice<T> slice) {
    return dedup(slice.start(), slice.end(), generic_is_equal_ptr<T>);
}

template <class T, class Is_Equal, class Set>
void dedup(cz::Slice<T>* slice, Is_Equal&& is_equal, Set&& set) {
    *slice = dedup(*slice, is_equal, set);
}
template <class T, class Is_Equal>
void dedup(cz::Slice<T>* slice, Is_Equal&& is_equal) {
    dedup(slice, is_equal, generic_set_ptr<T>);
}
template <class T>
void dedup(cz::Slice<T>* slice) {
    dedup(slice, generic_is_equal_ptr<T>);
}

template <class T, class Is_Equal, class Set>
void dedup(cz::Vector<T>* vector, Is_Equal&& is_equal, Set&& set) {
    cz::Slice<T> slice = dedup(vector->as_slice(), is_equal, set);
    vector->len = slice.len;
}
template <class T, class Is_Equal>
void dedup(cz::Vector<T>* vector, Is_Equal&& is_equal) {
    dedup(vector, is_equal, generic_set_ptr<T>);
}
template <class T>
void dedup(cz::Vector<T>* vector) {
    dedup(vector, generic_is_equal_ptr<T>);
}

}
