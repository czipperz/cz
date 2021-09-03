#pragma once

#include "allocator.hpp"
#include "string.hpp"
#include "vector.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <dirent.h>
#endif

namespace cz {

/// A `Directory_Iterator` iterates through the files in a directory.
///
/// The `files` functions list all the files in the given directory into a vector.
///
/// Example:
///
/// ```
/// cz::Buffer_Array buffer_array;
/// buffer_array.init();
/// CZ_DEFER(buffer_array.drop());
///
/// // Using `Str` because `buffer_array` owns the allocations.
/// cz::Vector<cz::Str> files = {};
/// CZ_DEFER(vector.drop(cz::heap_allocator()));
///
/// bool success = cz::files(cz::heap_allocator(), buffer_array.allocator(), path, &files);
/// ```
///
/// is similar to:
///
/// ```
/// cz::Buffer_Array buffer_array;
/// buffer_array.init();
/// CZ_DEFER(buffer_array.drop());
///
/// // Using `Str` because `buffer_array` owns the allocations.
/// cz::Vector<cz::Str> files = {};
/// CZ_DEFER(vector.drop(cz::heap_allocator()));
///
/// int result = 1;
///
/// cz::Directory_Iterator iterator;
/// result = iterator.init(path);
///
/// if (result == 1) while (1) {
///     cz::String file = iterator.str_name().clone(buffer_array.allocator());
///     files->reserve(cz::heap_allocator(), 1);
///     files->push(file);
///
///     result = iterator.advance();
///     if (result <= 0) {
///         if (result == 0) {
///             /* No more entries. */
///             success = iterator.drop();
///         } else {
///             /* Iteration failed in middle. */
///             iterator.drop();
///             success = false;
///         }
///         break;
///     }
/// }
///
/// // Result will be -1 on error, 0 on success.
/// ```
struct Directory_Iterator {
#ifdef _WIN32
    HANDLE directory;
    WIN32_FIND_DATAA entry;
#else
    DIR* directory;
    struct dirent* entry;
#endif

    /// Create the iterator for the files in the directory `path`.
    /// Returns `-1` on error, `0` on empty directory, `1` on success.
    int init(const char* path);

    /// Should only be called if `init` succeeds.
    /// Returns `false` on error.
    bool drop();

    /// Returns `1` if an entry was found, `0` at end of directory, `-1` on error.
    int advance();

    /// Get the name of the entry.
    const char* get_name() const;
    Str str_name() const;

    /// Append the name of the entry to the string and null terminates.
    void append_name(Allocator allocator, String* string) const;
};

/// Get the files in the directory `path`.  Each file is null terminated.
///
/// `path` should be expressed with forward slashes not
/// back slashes.  The path may be followed by a trailing slash.
///
/// `vector_allocator` is used to allocate the vector `files`
/// whereas `file_allocator` is used to allocate the file strings.
///
/// A common pattern is to use the `heap_allocator` as the `vector_allocator` and a
/// `Buffer_Array` as the `file_allocator`.  This will contiguously allocate the files in
/// the `Buffer_Array` which is great for performance and makes it super easy to clean up.
///
/// Returns `false` if any errors occur.  In this case you `files` is left in a
/// valid state so the results of iteration up to that point can still be used.
bool files(Allocator vector_allocator,
           Allocator file_allocator,
           const char* path,
           Vector<String>* files);

/// Same but uses `Str` instead of `String` for each file.
bool files(Allocator vector_allocator,
           Allocator file_allocator,
           const char* path,
           Vector<Str>* files);

/// Same but uses `const char*` instead of `String` for each file.
bool files(Allocator vector_allocator,
           Allocator file_allocator,
           const char* path,
           Vector<const char*>* files);

}
