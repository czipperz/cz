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
///
/// The path must use forward slashes.
Option<Str> directory_component(Str path);

/// Get the name component of the path.
///
/// If the last character of the string is a forward slash, none is returned.
/// If the input is empty, none is returned.
/// If there is both a directory and a name, the part after the trailing slash
/// is returned.
///
/// The path must use forward slashes.
Option<Str> name_component(Str path);

/// Flatten the path, removing `..` and `.` inplace.
/// Prefers leaving trailing `/` rather than removing them.
///
/// This will leave `..`s at the start since they cannot be flattened.
///
/// Does not put in a null terminator.
///
/// The path must use forward slashes.
void flatten(char* path, size_t* len);

/// See `flatten_path(char*, size_t*)`.
void flatten(String* path);

/// Test if the path is absolute.
///
/// On *nix, this means that it starts with `/`.  On Windows, this means that it
/// starts with `X:/` where `X` is a drive.
///
/// The path must use forward slashes.
bool is_absolute(Str path);

/// Make an absolute path out of the relative path.
///
/// If the input path is not absolute, we append it to the current working directory.
/// Then in either case we flatten the path.
///
/// The path must use forward slashes.
Result make_absolute(Str relative_path, Allocator allocator, String* absolute_path_out);

/// Convert the path to use forward slashes (`/`) instead of backward slashes (`\\`).
///
/// All the functions in this module rely on forward slashes being used as directory separators.
///
/// Note that this is only relevant to call on Windows (ie `_WIN32` is defined).
void convert_to_forward_slashes(char* path, size_t len);

/// Convert the path to use back slashes (`\\`) instead of forward slashes (`/`).
///
/// Note that this is only relevant to call on Windows (ie `_WIN32` is defined).
void convert_to_back_slashes(char* path, size_t len);

}
}
