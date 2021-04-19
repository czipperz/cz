#pragma once

#include <cz/allocator.hpp>
#include <cz/assert.hpp>
#include <cz/vector.hpp>
#include "next_power_of_two.hpp"

namespace cz {

template <class T>
struct Queue {
    T* elems;
    size_t offset;
    size_t len;
    size_t cap;

    void push_end(const T& t) {
        CZ_DEBUG_ASSERT(len + 1 <= cap);
        size_t end = (offset + len) & (cap - 1);
        elems[end] = t;
        ++len;
    }

    void push_start(const T& t) {
        CZ_DEBUG_ASSERT(len + 1 <= cap);
        offset = (offset + cap - 1) & (cap - 1);
        elems[offset] = t;
        ++len;
    }

    void append_end(cz::Slice<const T> slice) { return append_end(slice.elems, slice.len); }
    void append_end(const T* ts, size_t tl) {
        CZ_DEBUG_ASSERT(len + tl <= cap);
        size_t end = (offset + len) & (cap - 1);

        // Copy items until the end of the array.
        if (end < cap) {
            size_t underhanging = cap - end;

            // Fast case for when we don't need to copy to the start of the array as well.
            if (tl <= underhanging) {
                memcpy(elems + end, ts, tl * sizeof(T));
                len += tl;
                return;
            }

            memcpy(elems + end, ts, underhanging * sizeof(T));
            ts += underhanging;
            tl -= underhanging;
            len += underhanging;
        }

        // Copy items to the start of the array.
        CZ_DEBUG_ASSERT(tl <= offset);
        memcpy(elems, ts, tl * sizeof(T));
        len += tl;
    }

    void append_start(cz::Slice<const T> slice) { return append_start(slice.elems, slice.len); }
    void append_start(const T* ts, size_t tl) {
        CZ_DEBUG_ASSERT(len + tl <= cap);

        // Copy items to the start of the array.
        if (offset > 0) {
            // Fast case for when we don't need to copy to the end of the array as well.
            if (tl <= offset) {
                memcpy(elems + offset - tl, ts, tl * sizeof(T));
                offset -= tl;
                len += tl;
                return;
            }

            memcpy(elems, ts + tl - offset, offset * sizeof(T));
            tl -= offset;
            len += offset;
        }

        // Copy items to the end of the array.
        offset = cap - tl;
        memcpy(elems + offset, ts, tl * sizeof(T));
        len += tl;
    }

    T pop_start() {
        CZ_DEBUG_ASSERT(len >= 1);
        --len;
        size_t old_offset = offset;
        offset = (offset + 1) & (cap - 1);
        return elems[old_offset];
    }

    T pop_end() {
        CZ_DEBUG_ASSERT(len >= 1);
        --len;
        return elems[(offset + len) & (cap - 1)];
    }

    void remove(size_t index) {
        CZ_DEBUG_ASSERT(len >= 1);
        CZ_DEBUG_ASSERT(index < len);

        if (index + offset < cap) {
            // We are removing before the wrap around.  Shift elements forward.
            // _ _ a b c X e f
            // _ _ _ a b c e f
            memmove(elems + offset + 1, elems + offset, index * sizeof(T));
            offset = (offset + 1) & (cap - 1);
        } else {
            // We are removing after the wrap around.  Shift elements backwards.
            // c X e f _ _ a b
            // c e f _ _ _ a b
            size_t index2 = (offset + index) & (cap - 1);
            memmove(elems + index2, elems + index2 + 1, (len - (index + 1)) * sizeof(T));
        }

        --len;
    }

    void remove_range(size_t start, size_t end) {
        CZ_DEBUG_ASSERT(end >= start);
        CZ_DEBUG_ASSERT(len >= (end - start));
        CZ_DEBUG_ASSERT(end <= len);

        if (offset + end <= cap) {
            // Removing purely before the wrap around.  Shift elements forward.
            // f _ _ a b X X e
            // f _ _ _ _ a b e
            memmove(elems + offset + (end - start), elems + offset, (end - start) * sizeof(T));
            offset = (offset + (end - start)) & (cap - 1);
            len -= (end - start);
            return;
        }

        if (offset + start < cap) {
            // Removing partially before the wrap around.  Shift elements forward.
            // Note: the X at the start is handled in the case below.
            // X f _ _ a b X X
            // X f _ _ _ _ a b
            memmove(elems + (cap - start), elems + offset, start * sizeof(T));
            end = (offset + end) & (cap - 1);
            offset = 0;
            len -= (cap - start - offset);
            start = 0;
            goto remove_after;
        }

        if (offset + end >= cap) {
        remove_after:
            // Removing after the wrap around.  Shift elements backwards.
            // c X e _ _ a b c
            // c e _ _ _ a b c
            size_t start2 = ((offset + start) & (cap - 1));
            size_t end2 = ((offset + end) & (cap - 1));
            memmove(elems + start2, elems + end2, (len - end) * sizeof(T));
            len -= (end - start);
        }
    }

    void reserve(cz::Allocator allocator, size_t extra) {
        size_t new_cap = len + extra;
        if (new_cap > cap) {
            CZ_DEBUG_ASSERT(new_cap >= 1);
            new_cap = next_power_of_two(new_cap - 1);

            T* new_elems = static_cast<T*>(
                allocator.realloc({elems, cap * sizeof(T)}, {new_cap * sizeof(T), alignof(T)}));
            CZ_ASSERT(new_elems);

            if (offset + len > cap) {
                size_t overhanging = offset + len - cap;
                size_t underhanging = cap - offset;
                if (overhanging <= underhanging) {
                    // 5__01234 -> ___012345_______
                    memcpy(new_elems + cap, new_elems, sizeof(T) * overhanging);
                } else {
                    // 2345__01 -> 2345__________01
                    memcpy(new_elems + new_cap - underhanging, new_elems + offset,
                           sizeof(T) * underhanging);
                    offset = new_cap - underhanging;
                }
            }

            elems = new_elems;
            cap = new_cap;
        }
    }

    void drop(cz::Allocator allocator) { allocator.dealloc({elems, cap * sizeof(T)}); }

    T& operator[](size_t index) {
        CZ_DEBUG_ASSERT(index < len);
        return elems[(offset + index) & (cap - 1)];
    }
    const T& operator[](size_t index) const {
        CZ_DEBUG_ASSERT(index < len);
        return elems[(offset + index) & (cap - 1)];
    }

    T& first() {
        CZ_DEBUG_ASSERT(len > 0);
        return elems[offset];
    }
    const T& first() const {
        CZ_DEBUG_ASSERT(len > 0);
        return elems[offset];
    }

    T& last() {
        CZ_DEBUG_ASSERT(len > 0);
        return elems[(offset + len - 1) & (cap - 1)];
    }
    const T& last() const {
        CZ_DEBUG_ASSERT(len > 0);
        return elems[(offset + len - 1) & (cap - 1)];
    }
};

}
