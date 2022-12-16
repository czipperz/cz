#pragma once

#include <stdint.h>
#include "str.hpp"

namespace cz {

/// The `parse` module.
/// As a user, use one of the combinators directly below.
/// To implement parsing the type T, overload `int64_t cz::parse(cz::Str str, T* out)`.
///
/// # Return value
/// The magnitude (absolute value) of the return value is the number of bytes that were consumed.
/// The sign is whether there was success.
/// 0 is returned if no bytes were consumed (total failure).

/// Basic parse combinator.
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

/// Same as `parse` but asserts that the entire string is consumed.
///
/// ```
/// int first, second;
/// cz::Str str = "10 - 23";
/// must_parse(str, &first, " - ", &second);
/// ```
template <class T1, class... Ts>
void must_parse(cz::Str str, T1 t1, Ts... ts) {
    int64_t result = parse(str, t1, ts...);
    CZ_ASSERT(result == str.len);
}

/// Same as `parse` but modifies `*str` to point to after the parsed region.
///
/// ```
/// int first, second;
/// cz::Str str = "10 - 23";
/// int64_t result = parse_advance(&str, &first, " - ");
/// CZ_ASSERT(result == strlen("10 - "));
/// must_parse(str, &second);
/// ```
template <class T1, class... Ts>
int64_t parse_advance(cz::Str* str, T1 t1, Ts... ts) {
    int64_t result = parse(*str, t1, ts...);
    if (result > 0)
        *str = str->slice_start(result);
    else
        *str = str->slice_start(-result);
    return result;
}

/// Same as `parse_advance` but asserts that there were no errors and some bytes were consumed.
///
/// ```
/// int first, second;
/// cz::Str str = "10 - 23";
/// must_parse_advance(&str, &first, " - ");
/// must_parse(str, &second);
/// ```
template <class T1, class... Ts>
void must_parse_advance(cz::Str* str, T1 t1, Ts... ts) {
    int64_t result = parse_advance(str, t1, ts...);
    CZ_ASSERT(result > 0);
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

///////////////////////////////////////////////////////////////////////////////

// clang-format off

/// Capture the rest of the output.
struct Parse_Rest { cz::Str* out; };
inline Parse_Rest rest(cz::Str* out) { return {out}; }
int64_t parse(cz::Str str, Parse_Rest rest);

/// Find `pattern` and capture the output before it.  If `pattern` is found then advances
/// to after the end of `pattern`.  If `pattern` isn't found, eat the entire string.
struct Parse_Until_Char { cz::Str* out; char pattern; };
struct Parse_Until_Str  { cz::Str* out; cz::Str pattern; };
inline Parse_Until_Char until(cz::Str* out, char pattern) { return {out, pattern}; }
inline Parse_Until_Str  until(cz::Str* out, cz::Str pattern) { return {out, pattern}; }
int64_t parse(cz::Str str, Parse_Until_Char until);
int64_t parse(cz::Str str, Parse_Until_Str  until);

/// Find `pattern` and capture the output before it.  If `pattern` is found then advances to after
/// the end of `pattern`.  If `pattern` isn't found, parse will fail and won't capture anything.
struct Parse_Before_Char { cz::Str* out; char pattern; };
struct Parse_Before_Str  { cz::Str* out; cz::Str pattern; };
inline Parse_Before_Char before(cz::Str* out, char pattern) { return {out, pattern}; }
inline Parse_Before_Str  before(cz::Str* out, cz::Str pattern) { return {out, pattern}; }
int64_t parse(cz::Str str, Parse_Before_Char before);
int64_t parse(cz::Str str, Parse_Before_Str  before);

/// `char*` gets converted to `cz::Str` so we need a custom overload for just one character.
struct Parse_Char { char* out; };
inline Parse_Char p_char(char* out) { return {out}; }
int64_t parse(cz::Str str, Parse_Char ch);

/// Ignore leading whitespace, then eat up until a whitespace
/// character (doesn't eat the whitespace character).
struct Parse_Word { cz::Str* out; };
inline Parse_Word word(cz::Str* out) { return {out}; }
int64_t parse(cz::Str str, Parse_Word word);

/// Ignore whitespace.
struct Parse_Skip_Space {};
inline Parse_Skip_Space skip_space() { return {}; }
int64_t parse(cz::Str str, Parse_Skip_Space skip);

// clang-format on

}
