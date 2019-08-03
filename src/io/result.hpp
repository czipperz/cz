#pragma once

#include <errno.h>
#include "../context_decl.hpp"
#include "../str.hpp"

namespace cz {
namespace io {

struct Result {
    Str message;

    bool is_ok() { return !is_err(); }
    bool is_err() { return message.buffer != 0; }

    constexpr static Result ok() { return {}; }

    static Result from_errno(C* c) { return from_errno(c, errno); }
    static Result from_errno(C* c, int e);
};

}

inline bool is_err(io::Result r) {
    return r.is_err();
}

}
