#pragma once

#include "allocator.hpp"
#include "string.hpp"

namespace cz {

/// Set the current working directory to `path`.  It should be
/// expressed with forward slashes.  Returns `false` on failure.
bool set_working_directory(const char* cstr_path);

/// Get the current working directory, appending it to `path`.
///
/// This will place a null terminator past the end of the path (allocating
/// memory for it).  The path will be expressed with forward slashes.
bool get_working_directory(Allocator allocator, String* path);

}
