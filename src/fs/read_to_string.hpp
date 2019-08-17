#pragma once

#include <stdio.h>
#include "../mem/allocator.hpp"
#include "../io/result.hpp"

namespace cz {
namespace fs {

io::Result read_to_string(mem::Allocator, String* string, FILE* file);
io::Result read_to_string(mem::Allocator, String* string, const char* cstr_file_name);
io::Result read_to_string_binary(mem::Allocator, String* string, const char* cstr_file_name);

}
}
