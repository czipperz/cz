#pragma once

#include "allocator.hpp"
#include "assert.hpp"
#include "next_power_of_two.hpp"
#include "slice.hpp"
#include "util.hpp"

namespace cz {

/// A dynamic allocated array.
///
/// # Example
///
/// ```
/// #include <cz/defer.hpp>
/// #include <cz/heap.hpp>
/// #include <cz/vector.hpp>
///
/// cz::Vector<int> vector = {};
/// CZ_DEFER(vector.drop(cz::heap_allocator()));
///
/// vector.reserve(cz::heap_allocator(), 2);
/// vector.push(42);
/// vector.push(-3);
/// ```
template <class T>
struct Vector {
    T* elems;
    size_t len;
    size_t cap;

    /// Deallocate the vector's memory.
    void drop(Allocator allocator) { allocator.dealloc(elems, cap); }

    ///
    /// Allocation methods.
    ///

    /// Ensure there are `extra` spaces available.  Amortizing expansion.
    void reserve(Allocator allocator, size_t extra) { reserve_total(allocator, len + extra); }
    void reserve_total(Allocator allocator, size_t total);

    /// Ensure there are `extra` spaces available.  Exact expansion.
    void reserve_exact(Allocator allocator, size_t extra) { reserve_exact_total(allocator, len + extra); }
    void reserve_exact_total(Allocator allocator, size_t total);

    /// Reallocate such that the capacity matches the length.
    void realloc(Allocator allocator) {
        T* new_elems = allocator.realloc(elems, cap, len);
        if (new_elems) {
            elems = new_elems;
            cap = len;
        }
    }

    /// Reallocates the vector to have `new_cap` as the new capacity.  If this
    /// causes the vector to shrink then the length will be adjusted accordingly.
    void resize(Allocator allocator, size_t new_cap) {
        T* new_elems = allocator.realloc(elems, cap, new_cap);
        if (new_elems) {
            elems = new_elems;
            cap = new_cap;
            if (cap < len) {
                len = cap;
            }
        }
    }

    /// Duplicate the vector.
    Vector clone(Allocator allocator) const {
        Vector result = {};
        result.reserve_exact_total(allocator, len);
        memcpy(result.elems, elems, len * sizeof(T));
        result.len = len;
        return result;
    }

    ///
    /// Insertion methods.
    /// Must `reserve` space before attempting to insert.
    /// Indices must be manually bounds checked.
    ///

    /// Push an element.
    void push(T t) {
        CZ_DEBUG_ASSERT(cap - len >= 1);
        elems[len] = t;
        ++len;
    }

    /// Append many elements.
    void append(Slice<const T> slice) {
        CZ_DEBUG_ASSERT(cap - len >= slice.len);
        memcpy(elems + len, slice.elems, slice.len * sizeof(T));
        len += slice.len;
    }

    /// Insert an element into the middle of the vector.
    void insert(size_t index, T t) {
        CZ_DEBUG_ASSERT(index <= len);
        CZ_DEBUG_ASSERT(cap - len >= 1);
        memmove(elems + index + 1, elems + index, (len - index) * sizeof(T));
        elems[index] = t;
        ++len;
    }

    /// Insert many elements into the middle of the vector.
    void insert_slice(size_t index, cz::Slice<const T> slice) {
        CZ_DEBUG_ASSERT(index <= len);
        CZ_DEBUG_ASSERT(cap - len >= slice.len);
        memmove(elems + index + slice.len, elems + index, (len - index) * sizeof(T));
        memcpy(elems + index, slice.elems, slice.len * sizeof(T));
        len += slice.len;
    }

    ///
    /// Removal methods.
    /// Indices must be manually bounds checked.
    ///

    /// Pop one element.  Note: must have an element to pop!
    T pop() {
        CZ_DEBUG_ASSERT(len >= 1);
        --len;
        return elems[len];
    }

    /// Remove an element from the middle of the vector.
    void remove(size_t index) {
        CZ_DEBUG_ASSERT(index < len);
        memmove(elems + index, elems + index + 1, sizeof(T) * (len - index - 1));
        --len;
    }

    /// Remove many elements from the middle of the vector.
    void remove_many(size_t index, size_t count) {
        CZ_DEBUG_ASSERT(index + count <= len);
        memmove(elems + index, elems + index + count, sizeof(T) * (len - index - count));
        len -= count;
    }

    /// Remove the range of elements from the middle of the vector.
    void remove_range(size_t start, size_t end) {
        CZ_DEBUG_ASSERT(end >= start);
        return remove_many(start, end - start);
    }

    ///
    /// Miscellaneous commands.
    ///

    constexpr size_t remaining() const { return cap - len; }

    /// Pointer iterators.
    T* start() { return elems; }
    constexpr const T* start() const { return elems; }
    T* begin() { return elems; }
    constexpr const T* begin() const { return elems; }
    T* end() { return elems + len; }
    constexpr const T* end() const { return elems + len; }

    /// Utility.
    T& first() { return get(0); }
    const T& first() const { return get(0); }
    T& last() {
        CZ_DEBUG_ASSERT(len > 0);
        return (*this)[len - 1];
    }
    const T& last() const {
        CZ_DEBUG_ASSERT(len > 0);
        return (*this)[len - 1];
    }

    /// Must manually bounds check!
    T& operator[](size_t i) { return get(i); }
    const T& operator[](size_t i) const { return get(i); }

    T& get(size_t i) {
        CZ_DEBUG_ASSERT(i < len);
        return elems[i];
    }
    const T& get(size_t i) const {
        CZ_DEBUG_ASSERT(i < len);
        return elems[i];
    }

    ///
    /// Slice methods
    ///

    operator Slice<T>() { return {elems, len}; }
    constexpr operator Slice<const T>() const { return {elems, len}; }

    Slice<T> as_slice() { return *this; }
    constexpr Slice<const T> as_slice() const { return *this; }

    Slice<T> slice(size_t start, size_t end) const { return {elems + start, end - start}; }
    Slice<T> slice(const T* start, size_t end) const { return slice(start - elems, end); }
    Slice<T> slice(size_t start, const T* end) const { return slice(start, end - elems); }
    Slice<T> slice(const T* start, const T* end) const { return slice(start - elems, end - elems); }

    Slice<T> slice_start(size_t start) const { return slice(start, len); }
    Slice<T> slice_start(const T* start) const { return slice(start, len); }

    Slice<T> slice_end(size_t end) const { return slice((size_t)0, end); }
    Slice<T> slice_end(const T* end) const { return slice((size_t)0, end); }
};

template <class T>
static void realloc_new_cap(Vector<T>* vector, Allocator allocator, size_t new_cap) {
    T* new_elems = allocator.realloc(vector->elems, vector->cap, new_cap);
    CZ_ASSERT(new_elems != nullptr);

    vector->elems = new_elems;
    vector->cap = new_cap;
}

template <class T>
void Vector<T>::reserve_total(Allocator allocator, size_t total) {
    if (cap < total) {
        size_t new_cap = next_power_of_two(total);
        if (new_cap < 8) {
            new_cap = 8;
        }

        realloc_new_cap(this, allocator, new_cap);
    }
}

template <class T>
void Vector<T>::reserve_exact_total(Allocator allocator, size_t total) {
    if (cap < total) {
        realloc_new_cap(this, allocator, total);
    }
}

template <class T>
Vector<T> Slice<T>::clone(Allocator allocator) const {
    T* ts = allocator.alloc<T>(len);
    CZ_ASSERT(ts);
    memcpy(ts, elems, sizeof(T) * len);
    return {ts, len, len};
}

}
