#pragma once

namespace cz {
namespace file {

/// Returns `true` iff there is a file at `path`.
bool exists(const char* path);

/// Returns `true` iff `path` exists and it is a directory.
bool is_directory(const char* path);

}
}
