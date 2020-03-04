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

    bool operator==(const Option& other) const {
        if (is_present) {
            return other.is_present && value == other.value;
        } else {
            return !other.is_present;
        }
    }
    bool operator!=(const Option& other) const { return !(*this == other); }
};

}
