#pragma once

#include <cz/string.hpp>

namespace cz {

void encode_hex(cz::Str input, cz::Allocator allocator, cz::String* output);
void decode_hex(cz::Str input, cz::Allocator allocator, cz::String* output);

void encode_base64(cz::Str input, cz::Allocator allocator, cz::String* output);
void decode_base64(cz::Str input, cz::Allocator allocator, cz::String* output);

}
