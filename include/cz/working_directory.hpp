#pragma once

#include "allocator.hpp"
#include "result.hpp"
#include "string.hpp"

namespace cz {

/// Set the current working directory to the null terminated string \c path.  It
/// should be expressed with forward slashes.
Result set_working_directory(const char* cstr_path);

/// Get the current working directory, storing it in the \c String \c path.
///
/// This will place a null terminator past the end of the path (allocating
/// memory for it).  The path will be expressed with forward slashes.
Result get_working_directory(Allocator allocator, String* path);

}
