#pragma once

#include <errno.h>
#include "../string.hpp"

namespace cz {
namespace io {

struct Result {
    Str message;

    bool is_ok() { return !is_err(); }
    bool is_err() { return message.buffer != 0; }

    constexpr static Result ok() { return {}; }
    constexpr static Result err(Str message) { return {message}; }

    static Result from_errno() { return from_errno(errno); }
    static Result from_errno(int e);
};

}

inline bool is_err(io::Result r) {
    return r.is_err();
}

}
