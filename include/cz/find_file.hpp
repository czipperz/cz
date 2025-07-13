#pragma once

#include "allocator.hpp"
#include "string.hpp"

namespace cz {

/// Starting at `path`, walks up the directory chain until it finds a relative path `file`.
/// Returns `true` if a file was found and `false` otherwise.
/// Stores the full null-terminated path in `path`.
bool find_file_up(Allocator allocator, String* path, Str file);

/// `find_dir_with_file_up` returns just the directory that
/// contains the file (without a trailing forward slash).
bool find_dir_with_file_up(Allocator allocator, String* path, Str file);

}
