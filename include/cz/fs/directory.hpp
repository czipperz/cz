#pragma once

#include "../allocator.hpp"
#include "../result.hpp"
#include "../string.hpp"
#include "../vector.hpp"

namespace cz {
namespace fs {

class DirectoryIterator {
    Allocator _allocator;
    String _file;
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
Result files(Allocator allocator, const char* cstr_path, Vector<String>* paths);

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
void flatten_path(char* buffer, size_t* len);

}
}
