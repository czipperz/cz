#pragma once

#include <utility>

namespace cz {

template <class T>
void swap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

template <class T>
T take(T& p) {
    T temp = T();
    swap(temp, p);
    return temp;
}

template <class T>
T replace(T& t, T val) {
    swap(t, val);
    return val;
}

using std::forward;
using std::move;

template <class T>
T max(T a, T b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

template <class T>
T min(T a, T b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

}
