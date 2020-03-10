#pragma once

#include "../allocator.hpp"
#include "../result.hpp"
#include "../string.hpp"
#include "../vector.hpp"

namespace cz {
namespace fs {

class DirectoryIterator {
    Allocator _allocator;
    String _file = {};
    bool _done = false;

#ifdef _WIN32
    /* HANDLE */ void* _handle /* uninitialized */;
#else
    /* DIR* */ void* _dir /* uninitialized */;
#endif

public:
    DirectoryIterator(Allocator allocator) : _allocator(allocator) {}

    Str file() const { return _file; }
    bool done() const { return _done; }

    /// Create the iterator for the files in the directory \c cstr_path.
    ///
    /// The path may be followed by a \c / .
    Result create(const char* cstr_path);
    Result advance();
    Result destroy();
};

/// Get the files in the directory \c cstr_path.
///
/// Path should be expressed with forward slashes not back slashes.  The path
/// may be followed by a trailing slash.
Result files(Allocator paths_allocator,
             Allocator path_allocator,
             const char* cstr_path,
             Vector<String>* paths);

}
}
