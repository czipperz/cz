#pragma once

#include "../file.hpp"
#include "../string.hpp"
#include "dwim.hpp"

namespace cz {
namespace dwim {

/// Read an `Input_File` to a string, returning `true` on success and `false` on failure.
bool read_to_string(Dwim* dwim, Input_File* file, String* output);

/// Read a file to a string allocated in the `dwim`.
String read_file(Dwim* dwim, const char* path);
bool read_file(Dwim* dwim, const char* path, cz::String* output);

}
}