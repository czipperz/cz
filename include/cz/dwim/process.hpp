#pragma once

#include "../process.hpp"
#include "../string.hpp"
#include "dwim.hpp"

namespace cz {
namespace dwim {

/// Run a script and read the output (stdout and stderr) to a string allocated in the `dwim`.
String run_script(Dwim* dwim, const char* script);

/// Run a script and capture stdout/stderr.
bool run_script(const char* script, Allocator allocator, String* output);

}
}
