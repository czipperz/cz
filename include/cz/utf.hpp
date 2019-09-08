#pragma once

#include <stddef.h>
#include <stdint.h>

namespace cz {
namespace utf32 {

/// Convert the code point to utf8.
///
/// Returns the number of bytes written.  The code point argument must be a
/// valid utf32 code point.  The output buffer must be at least 4 bytes long to
/// prevent buffer overruns.
size_t to_utf8(uint32_t code_point, uint8_t out_buffer[4]);

}

namespace utf8 {

/// Check if the buffer is valid utf8.
///
/// Does do bounds checking.
bool is_valid(const uint8_t* buffer, size_t len);

/// Get the next code point in utf32 format.
///
/// The buffer must be valid utf8.  Does not do bounds checking.
uint32_t to_utf32(const uint8_t* utf8_buffer);

/// Get the forward offset of the next code point.  The argument should point to
/// the start of the code point you are interested in.
///
/// The buffer must be valid utf8.  This function does not do bounds checking.
size_t forward(const uint8_t* utf8_buffer);

/// Get the backward offset of the previous code point.  The argument should
/// point to the end of the code point you are interested in.
///
/// The buffer must be valid utf8.  This function does not do bounds checking.
size_t backward(const uint8_t* utf8_buffer);

}
}
