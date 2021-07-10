#pragma once

namespace cz {

template <class T>
bool generic_is_less_ptr(T* left, T* right) {
    return *left < *right;
}

template <class T>
bool generic_is_equal_ptr(T* left, T* right) {
    return *left == *right;
}

template <class T>
void generic_swap_ptr(T* left, T* right) {
    T temp = *left;
    *left = *right;
    *right = temp;
}

template <class T>
void generic_set_ptr(T* left, T* right) {
    *left = *right;
}

}
