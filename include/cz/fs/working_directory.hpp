#pragma once

#include "../io/result.hpp"
#include "../mem/allocator.hpp"

namespace cz {
namespace fs {

/// Set the current working directory to the null terminated string \c path.  It
/// should be expressed with forward slashes.
io::Result set_working_directory(const char* cstr_path);

/// Get the current working directory, storing it in the \c String \c path.
///
/// This will place a null terminator past the end of the path (allocating
/// memory for it).  The path will be expressed with forward slashes.
io::Result get_working_directory(mem::Allocator allocator, String* path);

}
}
