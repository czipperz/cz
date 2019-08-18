#pragma once

#include "../io/result.hpp"
#include "../mem/allocator.hpp"

namespace cz {
namespace fs {

/// Set the current working directory to the null terminated string \c path.
io::Result set_working_directory(const char* cstr_path);

/// Append the current working directory to the \c String \c path.  This will
/// place a null terminator past the end of the path (allocating memory for it).
io::Result get_working_directory(mem::Allocator allocator, String* path);

}
}
