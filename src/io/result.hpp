#pragma once

#include "../context_decl.hpp"
#include "../str.hpp"

namespace cz {
namespace io {

struct Result {
    int code;

    constexpr static Result ok() { return {0}; }
    static Result last_error();

    constexpr bool is_ok() const { return !is_err(); }
    constexpr bool is_err() const { return code != 0; }
};

}

inline bool is_err(io::Result r) {
    return r.is_err();
}

}
