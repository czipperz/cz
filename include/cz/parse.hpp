#pragma once

#include <stdint.h>
#include "str.hpp"

namespace cz {

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

}
