#pragma once

#include "../process.hpp"
#include "../string.hpp"
#include "dwim.hpp"

namespace cz {
namespace dwim {

/// Read an `Input_File` to a string, returning `true` on success and `false` on failure.
bool read_to_string(Dwim* dwim, Input_File* file, String* output);

/// Read a file to a string allocated in the `dwim`.
String read_file(Dwim* dwim, const char* path);

/// Run a script and read the output (stdout and stderr) to a string allocated in the `dwim`.
String run_script(Dwim* dwim, const char* script);

/// Run a script and capture stdout/stderr.
bool run_script(const char* script, Allocator allocator, String* output);

}
}
