#pragma once

#include "allocator.hpp"
#include "result.hpp"
#include "string.hpp"
#include "vector.hpp"

namespace cz {

/// A `Directory_Iterator` iterates through the files in a
/// directory.  For example code see implementations of `files`.
struct Directory_Iterator {
    bool _done = false;

#ifdef _WIN32
    /* HANDLE */ void* _handle /* uninitialized */;
#else
    /* DIR* */ void* _dir /* uninitialized */;
#endif

    bool done() const { return _done; }

    /// Create the iterator for the files in the directory `path`.
    /// Finds the first entry and appends it to `file`.  Doesn't null terminate.
    Result init(const char* path, Allocator allocator, String* file);

    Result drop();

    /// Advance and append the next entry to `file`.  Doesn't null terminate.
    Result advance(Allocator allocator, String* file);
};

/// Get the files in the directory `cstr_path`.
///
/// `cstr_path` should be expressed with forward slashes not
/// back slashes.  The path may be followed by a trailing slash.
///
/// `vector_allocator` is used to allocate the vector `files`
/// whereas `file_allocator` is used to allocate the file strings.
///
/// A common pattern is to use the `heap_allocator` as the `vector_allocator` and a
/// `Buffer_Array` as the `file_allocator`.  This will contiguously allocate the files in
/// the `Buffer_Array` which is great for performance and makes it super easy to clean up.
Result files(Allocator vector_allocator,
             Allocator file_allocator,
             const char* cstr_path,
             Vector<String>* files);

/// Same but uses `Str` instead of `String` for each file.
Result files(Allocator vector_allocator,
             Allocator file_allocator,
             const char* cstr_path,
             Vector<Str>* files);

/// Same but allocates a null terminator character after each string.
Result files_null_terminate(Allocator vector_allocator,
                            Allocator file_allocator,
                            const char* cstr_path,
                            Vector<String>* files);

/// Same but allocates a null terminator character after each string and stores them as `Str`.
Result files_null_terminate(Allocator vector_allocator,
                            Allocator file_allocator,
                            const char* cstr_path,
                            Vector<Str>* files);

}
