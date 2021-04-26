#pragma once

#include <string>
#include "defer.hpp"
#include "format.hpp"

namespace cz {

template <class... Ts>
std::string std_string_format(Ts... ts) {
    cz::Heap_String string = format(ts...);
    CZ_DEFER(string.drop());

    std::string std_string;
    std_string.assign(string.buffer(), string.len());
    return std_string;
}

inline std::string std_string_format(void* t, ...) {
    return std_string_format(address(t));
}

}
