/// Functions for manipulating paths.
///
/// This module only manipulates paths formatted with forward slashes (`/`).  If on
/// Windows you should use `convert_to_forward_slashes` to clean input strings.

#pragma once

#include "option.hpp"
#include "string.hpp"

namespace cz {
class Heap_String;

namespace path {

/// Get the max length of a path on the current platform.  On failure
/// returns `false` and **does not store to `*size`**.
bool get_max_len(size_t* size);

/// Get the directory component of the path.
///
/// If the path doesn't contain a forward slash, returns `false`.
/// Otherwise, returns the section before the last forward slash.
///
/// The path must use forward slashes.
bool directory_component(Str path, size_t* directory_end);
bool directory_component(Str path, Str* directory);

/// Run `directory_component` inline on the path.
bool pop_component(Str* path);
bool pop_component(String* path);

/// Same as `pop_component` but keeps the trailing `/`.  Note that
/// calling `pop_name` more than one time will do nothing.
bool pop_name(Str path, size_t* end);
bool pop_name(Str* path);
bool pop_name(String* path);

/// Add a component to the string.  This generally will append `/name`,
/// but if the `path` has a trailing `/` then it'll just append `name`.
void push_component(Allocator allocator, String* path, Str name);
void push_component(Heap_String* path, Str name);

/// Get the name component of the path.
///
/// If the path ends in a forward slash, returns `false`.
/// Otherwise, if the path contains a forward slash,
/// returns the section after the last forward slash.
/// Otherwise, returns the path.
///
/// The path must use forward slashes.
bool name_component(Str path, size_t* name_start);
bool name_component(Str path, Str* name);

/// Test if the path has the component.
bool has_component(Str path, Str component);

/// Flatten the path, removing `..` and `.` inplace.
/// Prefers leaving trailing `/` rather than removing them.
///
/// This will leave `..`s at the start since they cannot be flattened.
///
/// Does not put in a null terminator.
///
/// The path must use forward slashes.
void flatten(char* path, size_t* len);

/// See `flatten(char*, size_t*)`.
void flatten(String* path);

/// Test if the path is absolute.
///
/// On *nix, this means that it starts with `/`.  On Windows, this means that
/// it starts with `X:/` where `X` is an alphabetic character (drive name).
/// See [https://docs.microsoft.com/en-us/dotnet/standard/io/file-path-formats].
///
/// The path must use forward slashes.
bool is_absolute(Str path);

/// A UNC path is of the form `//server/share/path`.  UNC paths are used
/// for network file shares, links, and sometimes for mounting disks.
///
/// This function is only relevant for paths on Windows.
bool is_unc_path(Str file);

/// Make an absolute path out of the relative path.
///
/// If the input path is not absolute, we append it to the current working directory.
/// Then in either case we flatten the path.
///
/// The path must use forward slashes.
///
/// Returns an error if it fails to get the current working directory.
bool make_absolute(Str relative_path, Allocator allocator, String* absolute_path_out);

/// The same as `make_absolute` above except with a custom working directory.
void make_absolute(Str relative_path,
                   Str working_directory,
                   Allocator allocator,
                   String* absolute_path_out);

/// Convert the path to use forward slashes (`'/'`) instead of backward slashes (`'\\'`).
///
/// This is only relevant on Windows as other platforms only use forward slashes.
void convert_to_forward_slashes(char* path, size_t len);

inline void convert_to_forward_slashes(String* path) {
    convert_to_forward_slashes(path->buffer, path->len);
}

/// Convert the path to use back slashes (`'\\'`) instead of forward slashes (`'/'`).
///
/// This is only relevant on Windows as other platforms only use forward slashes.
void convert_to_back_slashes(char* path, size_t len);

inline void convert_to_back_slashes(String* path) {
    convert_to_back_slashes(path->buffer, path->len);
}

/// Test if the character is a directory separator on this platform.
inline bool is_dir_sep(char ch) {
#ifdef _WIN32
    if (ch == '\\') {
        return true;
    }
#endif

    return ch == '/';
}

}
}
