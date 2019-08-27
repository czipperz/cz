#pragma once

#include "../io/result.hpp"
#include "../mem/allocator.hpp"
#include "../string.hpp"
#include "../vector.hpp"

namespace cz {
namespace fs {

class DirectoryIterator {
    mem::Allocator _allocator;
    String _file;
    bool _done = false;

#ifdef _WIN32
    /* HANDLE */ void* _handle /* uninitialized */;
#else
    /* DIR* */ void* _dir /* uninitialized */;
#endif

public:
    DirectoryIterator(mem::Allocator allocator) : _allocator(allocator) {}

    Str file() const { return _file; }
    bool done() const { return _done; }

    /// Create the iterator for the files in the directory \c cstr_path.
    ///
    /// The path may be followed by a \c / .
    io::Result create(const char* cstr_path);
    io::Result advance();
    io::Result destroy();
};

/// Get the files in the directory \c cstr_path.
///
/// Path should be expressed with forward slashes not back slashes.  The path
/// may be followed by a trailing slash.
io::Result files(mem::Allocator allocator, const char* cstr_path, Vector<String>* paths);

/// Get the directory component of the path, including the trailing slash.
///
/// Returns empty string if it is just a file name.  Does nothing if the string
/// ends in /.
Str directory_component(Str);

}
}
