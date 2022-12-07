#pragma once

#include <stdint.h>
#include "str.hpp"

namespace cz {

/// Parse combinator.  Example usage:
///
/// ```
/// int first, second;
/// cz::Str str = "10 - 23";
/// int64_t result = parse(str, &first, " - ", &second);
/// CZ_ASSERT(result == str.len);
/// ```
template <class T1, class T2, class... Ts>
int64_t parse(cz::Str str, T1 t1, T2 t2, Ts... ts) {
    int64_t r1 = parse(str, t1);
    if (r1 <= 0)
        return r1;

    int64_t r2 = parse(str.slice_start(r1), t2, ts...);
    if (r2 <= 0)
        return -r1 + r2;

    return r1 + r2;
}

///////////////////////////////////////////////////////////////////////////////

/// If `str` starts with `pattern` then return `pattern.len` (or 1 for chars).
/// Otherwise returns `0`.
int64_t parse(cz::Str str, cz::Str pattern);
int64_t parse(cz::Str str, char pattern);

///////////////////////////////////////////////////////////////////////////////

/// Parse a string to a number, returning the number of characters consumed.
///
/// On success, `x` is returned where the number was `x` characters long.
/// If the string does not start with a number then `0` is returned.
/// If the number overflows then `-x` is returned.
int64_t parse(cz::Str str, uint8_t* out);
int64_t parse(cz::Str str, uint16_t* out);
int64_t parse(cz::Str str, uint32_t* out);
int64_t parse(cz::Str str, uint64_t* out);

int64_t parse(cz::Str str, int8_t* out);
int64_t parse(cz::Str str, int16_t* out);
int64_t parse(cz::Str str, int32_t* out);
int64_t parse(cz::Str str, int64_t* out);

// Apple defines `uint64_t` as `unsigned long long` but `size_t` as `unsigned long` even
// though both are 8 bytes.  The compiler doesn't know which overload above to choose so
// it errors out if we don't provide these extra overloads for the missing int sizes.
#if __APPLE__ && __SIZEOF_SIZE_T__ == 8
inline int64_t parse(cz::Str str, long* out) {
    return parse(str, (int64_t*)out);
}
inline int64_t parse(cz::Str str, unsigned long* out) {
    return parse(str, (uint64_t*)out);
}
#endif

}
