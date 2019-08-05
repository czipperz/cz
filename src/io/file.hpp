#pragma once

#include <stdio.h>
#include "../context.hpp"
#include "result.hpp"

namespace cz {
namespace io {

Result read_to_string(C* c, String* string, FILE* file);
Result read_to_string(C* c, String* string, const char* cstr_file_name);

}
}
