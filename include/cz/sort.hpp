#pragma once

#include <cz/template_generic.hpp>
#include <cz/slice.hpp>
#include <cz/vector.hpp>

namespace cz {

template <class Iterator, class Is_Less, class Swap>
void generic_insertion_sort(Iterator start, Iterator end, Is_Less&& is_less, Swap&& swap) {
    Iterator middle = start;
    for (++middle; middle < end; ++middle) {
        Iterator point = middle;
        while (point != start) {
            Iterator prev = point;
            --point;

            if (is_less(prev, point)) {
                // Shift element at middle left until we reach an element that is not less than it.
                swap(point, prev);
            } else {
                break;
            }
        }
    }
}

template <class Iterator, class Is_Less, class Swap>
void generic_sort(Iterator start, Iterator end, Is_Less&& is_less, Swap&& swap) {
    if (start + 8 >= end) {
        generic_insertion_sort(start, end, is_less, swap);
        return;
    }

    // Find pivot
    Iterator pivot;
    {
        Iterator middle = start + (end - start) / 2;
        if (is_less(start, middle)) {
            // S M
            if (is_less(middle, end - 1)) {
                // S M E
                pivot = middle;
            } else if (is_less(start, end - 1)) {
                // S E M
                pivot = end - 1;
            } else {
                // E S M
                pivot = start;
            }
        } else {
            // M S
            if (is_less(start, end - 1)) {
                // M S E
                pivot = start;
            } else if (is_less(middle, end - 1)) {
                // M E S
                pivot = end - 1;
            } else {
                // E M S
                pivot = middle;
            }
        }
    }

    // Partition using pivot
    {
        swap(pivot, end - 1);
        pivot = end - 1;

        Iterator i = start;
        for (Iterator j = start; j < end - 1; ++j) {
            if (is_less(j, pivot)) {
                swap(i, j);
                ++i;
            }
        }

        pivot = i;
        swap(pivot, end - 1);
    }

    // Recurse left and right
    generic_sort(start, pivot, is_less, swap);
    generic_sort(pivot + 1, end, is_less, swap);
}

template <class T, class Is_Less, class Swap>
void sort(cz::Slice<T> slice, Is_Less&& is_less, Swap&& swap) {
    generic_sort(slice.start(), slice.end(), is_less, swap);
}
template <class T, class Is_Less>
void sort(cz::Slice<T> slice, Is_Less&& is_less) {
    sort(slice, is_less, generic_swap_ptr<T>);
}
template <class T>
void sort(cz::Slice<T> slice) {
    sort(slice, generic_is_less_ptr<T>);
}

template <class T, class Is_Less, class Swap>
void sort(cz::Vector<T> vector, Is_Less&& is_less, Swap&& swap) {
    sort(vector.as_slice(), is_less, swap);
}
template <class T, class Is_Less>
void sort(cz::Vector<T> vector, Is_Less&& is_less) {
    sort(vector.as_slice(), is_less);
}
template <class T>
void sort(cz::Vector<T> vector) {
    sort(vector.as_slice());
}

template <class Iterator, class Is_Less>
bool generic_is_sorted(Iterator start, Iterator end, Is_Less&& is_less) {
    Iterator next = start;
    ++next;

    while (next < end) {
        // start can be <= next so we test if start > next == next < start.
        if (is_less(next, start)) {
            return false;
        }

        start = next;
        ++next;
    }

    return true;
}

template <class T, class Is_Less>
bool is_sorted(cz::Slice<T> slice, Is_Less&& is_less) {
    return generic_is_sorted(slice.start(), slice.end(), is_less);
}
template <class T>
bool is_sorted(cz::Slice<T> slice) {
    return is_sorted(slice, generic_is_less_ptr<T>);
}
template <class T, class Is_Less>
bool is_sorted(cz::Vector<T> vector, Is_Less&& is_less) {
    return is_sorted(vector.as_slice(), is_less);
}
template <class T>
bool is_sorted(cz::Vector<T> vector) {
    return is_sorted(vector.as_slice());
}

}
