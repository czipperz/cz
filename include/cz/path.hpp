/// \file path.hpp
///
/// Functions for manipulating paths.
///
/// Paths are complicated and have different cross-platform behavior.  This
/// module aims to abstract over the basic concepts without making them
/// complicated to work with.  To do this we use forward slashes for all paths.
///
/// On *nix absolute paths start with \c / .  On Windows absolute paths are
/// separate from drive specifiers.  Thus all of the following are valid: \c
/// C:/x/y , \c C:x/y , \c /x/y , \c x/y .
/// https://docs.microsoft.com/en-us/dotnet/standard/io/file-path-formats

#pragma once

#include "option.hpp"
#include "result.hpp"
#include "string.hpp"

namespace cz {
namespace path {

/// Get the max length of a path on the current platform.  Does not write to the
/// argument if there is no maximum.
Result get_max_len(size_t* size);

/// Get the directory component of the path, including the trailing slash.
///
/// Returns none if there is no directory (no forward slash).
Option<Str> directory_component(Str path);

/// Get the name component of the path.
///
/// If the last character of the string is a forward slash, none is returned.
/// If the input is empty, none is returned.
/// If there is both a directory and a name, the part after the trailing slash
/// is returned.
Option<Str> name_component(Str path);

/// Flatten the path, removing \c .. and \c . inplace.
/// Prefers leaving trailing \c / rather than removing them.
///
/// This will leave \c .. s at the start since they cannot be flattened.
///
/// Does not put in a null terminator.
void flatten(char* path, size_t* len);

/// See \c flatten_path(char*, size_t*) .
void flatten(String* path);

/// Test if the path is absolute.
///
/// On *nix, this means that it starts with \c / .  On Windows, this means that it
/// starts with \c X:/ where \c X is a drive.
bool is_absolute(Str path);

/// Make an absolute path out of the relative path.
Result make_absolute(Str relative_path, Allocator allocator, String* absolute_path_out);

}
}
