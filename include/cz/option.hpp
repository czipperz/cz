#pragma once

namespace cz {

template <class T>
struct Option {
    union {
        T value;
    };
    bool is_present;

    Option() : is_present(false) {}
    Option(T value) : value(value), is_present(true) {}
};

}
