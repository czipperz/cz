#pragma once

#include "allocator.hpp"
#include "string.hpp"

namespace cz {

/// Starting at `path`, walks up the directory
/// chain until it finds a relative path `file`.
///
/// Returns `true` if a file was found and `false` otherwise.
///
/// `find_file_up` returns (in `path`) the full path to the file that is found.
/// `find_dir_with_file_up` returns just the directory that contains the file.
///
/// The output path is null terminated in either case.
bool find_file_up(Allocator allocator, String* path, Str file);
bool find_dir_with_file_up(Allocator allocator, String* path, Str file);

}
