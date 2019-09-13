#pragma once

#include <stdio.h>
#include "../allocator.hpp"
#include "../result.hpp"
#include "../string.hpp"

namespace cz {
namespace fs {

Result read_to_string(Allocator, String* string, FILE* file);
Result read_to_string(Allocator, String* string, const char* cstr_file_name);
Result read_to_string_binary(Allocator, String* string, const char* cstr_file_name);

}
}
