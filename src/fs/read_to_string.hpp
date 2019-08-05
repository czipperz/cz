#pragma once

#include <stdio.h>
#include "../context.hpp"
#include "../io/result.hpp"

namespace cz {
namespace fs {

io::Result read_to_string(C* c, String* string, FILE* file);
io::Result read_to_string(C* c, String* string, const char* cstr_file_name);

}
}
