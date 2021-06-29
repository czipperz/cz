#pragma once

namespace cz {
namespace file {

/// Returns `true` iff there is a file at `path`.
bool exists(const char* path);

/// Returns `true` iff `path` exists and it is a directory.
bool is_directory(const char* path);

/// Returns `true` iff `path` exists and it is a directory and it is not a symlink.
bool is_directory_and_not_symlink(const char* path);

/// Try to create a directory at `path`.
///
/// On success returns 0.
/// If there is already a file at this path then return 2.
/// On other failures returns 1.
int create_directory(const char* path);

}
}
