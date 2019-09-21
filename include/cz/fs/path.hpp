#pragma once

#include "../fs.hpp"

namespace cz {
namespace fs {

/// Get the directory component of the path, including the trailing slash.
///
/// Returns empty string if it is just a file name.  Does nothing if the string
/// ends in /.
Str directory_component(Str);

/// Flatten the path, removing \c .. and \c . inplace.
/// Prefers leaving trailing \c / rather than removing them.
///
/// This will leave \c .. s at the start since they cannot be flattened.
///
/// Does not put in a null terminator.
void flatten_path(char* path, size_t* len);

/// See \c flatten_path(char*, size_t*) .
void flatten_path(String* path);

/// Test if the path is absolute.
///
/// On linux, this means that it starts with \c / .  On Windows, this means that it
/// starts with \c X:/ where \c X is a drive.
bool is_absolute(Str path);

/// Make an absolute path out of the relative path.
Result make_absolute(Str relative, Allocator allocator, String* path);

}
}
