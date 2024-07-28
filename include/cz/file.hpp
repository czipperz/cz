#pragma once

#include <stdint.h>
#include "string.hpp"

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

/// Remove an empty directory.  Returns `true` if successful.
bool remove_empty_directory(const char* path);

/// Remove a non-directory file.  Returns `true` if successful.
bool remove_file(const char* path);

/// Move a file to a new path.  Returns `true` if successful.
///
/// I have observed:
/// On Windows, if `new_path` already exists, does nothing and returns `false`.
/// On Linux, if `new_path` already exists, replaces the file at `new_path` and returns `true`.
bool rename_file(const char* old_path, const char* new_path);

}

namespace Relative_To_ {
/// An enum describing what `File_Descriptor::set_position()` will set the position relative to.
enum Relative_To {
    /// The start of the file.  The value must not be negative.
    START,

    /// The end of the file.  The value must not be positive.
    END,

    /// The current point in the file.  The value, once added to
    /// the current position of the file, should not be negative.
    ///
    /// If the sum is greater than the size of the file and you have an `Output_File`
    /// you can then write to it; on Windows this will leave the intermediate bytes
    /// uninitialized whereas on Linux it will fill them with `'\0'`.
    CURRENT,
};
}
using Relative_To_::Relative_To;

struct File_Descriptor {
#ifdef _WIN32
    void* handle = Null_;

    /// Initialize the `HANDLE` to `INVALID_HANDLE_VALUE`.  Note that `nullptr` is also an invalid
    /// `HANDLE` but we don't check for that because it's uncommon for Win32 to return `NULL`.
    static constexpr void* const Null_ = (void*)-1;
#else
    int handle = Null_;

    /// Any fd < 0 is invalid on Unix systems.
    static constexpr const int Null_ = -1;
#endif

    /// If the `File_Descriptor` was successfully opened, then close it.
    /// Note: this function does not reset the state of the `File_Descriptor`.
    void close();

    /// Check if the `File_Descriptor` is open.
    bool is_open() const;

    /// Set if the file should block on IO operations or immediately fail.  The default is blocking.
    ///
    /// For `Input_File`s this allows for a "poll" if input is available.
    /// For `Output_File`s this causes `write`s to fail when the buffer is full.
    bool set_non_blocking();
    bool set_blocking();

    /// Set if the file is open in a child process.
    /// Inheritable is the default and means it will be.
    bool set_non_inheritable();
    bool set_inheritable();

    /// Set the position of the file relative to a marker (the
    /// start or end of the file or the current position).
    ///
    /// Returns `-1` on failure.  On success returns the position
    /// of the file (relative to the start of the file).
    ///
    /// This is referred to as "seeking" on some filesystems.
    int64_t set_position(int64_t value, Relative_To relative_to);

    /// Get the position of the file or `-1` on error.
    int64_t get_position() {
        return set_position(0, Relative_To::CURRENT);
    }

    /// Get the size of the file or `-1` on error.
    int64_t get_size();
};

struct Carriage_Return_Carry {
    bool carrying = false;
};

/// Strip carriage returns from the given string.  Doesn't handle trailing carriage returns.
void strip_carriage_returns(char* buffer, size_t* size);
inline static void strip_carriage_returns(String* string);

/// Attempts to strip a trailing carriage return.  Stores into
/// `carry` whether there was a trailing return or not.
void strip_trailing_carriage_return(char* buffer, size_t* size, Carriage_Return_Carry* carry);
inline static void strip_trailing_carriage_return(String* string, Carriage_Return_Carry* carry);

/// Strip carriage returns from the string, storing into
/// `carry` whether there was a trailing carriage return.
inline static void strip_carriage_returns(char* buffer, size_t* size, Carriage_Return_Carry* carry);
inline static void strip_carriage_returns(cz::String* string, Carriage_Return_Carry* carry);

struct Input_File : File_Descriptor {
    /// Try to open a file for reading.
    ///
    /// Returns `true` if it succeeds, `false` otherwise.
    bool open(const char* file);

    /// Read up to `size` bytes from the file into `buffer`.
    ///
    /// Returns the number of bytes read, which is equivalent
    /// to the number of bytes written to `buffer`.
    ///
    /// On failure returns `-1`.  On end of file returns `0`.
    int64_t read(void* buffer, size_t size);

    /// Read up to `size` bytes from the file into `buffer`.
    ///
    /// Returns the number of bytes written to `buffer`.  This may be less than
    /// the number of bytes read if any `"\r\n"` sequences were encountered.
    ///
    /// On failure returns `-1`.  On end of file returns `0`.
    int64_t read_strip_carriage_returns(char* buffer, size_t size, Carriage_Return_Carry*);

    /// Wrapper for `read` and `read_strip_carriage_returns` that
    /// selects the implementation based on the host operating system.
    int64_t read_text(char* buffer, size_t size, Carriage_Return_Carry* carry) {
#ifdef _WIN32
        return read_strip_carriage_returns(buffer, size, carry);
#else
        (void)carry;  // remove unused parameter warnings
        return read(buffer, size);
#endif
    }
};

struct Output_File : File_Descriptor {
    /// Try to open a file for writing.
    ///
    /// Returns `true` if it succeeds, `false` otherwise.
    ///
    /// If the file exists, it removes the existing contents.
    /// If the file doesn't exist, it creates the file.
    bool open(const char* file);

    /// Write `size` bytes from `buffer` to the file.
    ///
    /// Returns the number of bytes written, which is equivalent
    /// to the number of bytes from `buffer` written.
    ///
    /// On failure returns `-1`.  On end of file returns `0`.
    int64_t write(const void* buffer, size_t size);
    int64_t write(cz::Str str) { return write(str.buffer, str.len); }

    /// Write `size` bytes from `buffer` to the file, converting each `'\n'` to `"\r\n"`.
    ///
    /// Returns the number of bytes from `buffer` that were written.  This may be
    /// less than the number of bytes written if any newlines were encountered.
    ///
    /// On failure returns `-1`.  On end of file returns `0`.
    int64_t write_add_carriage_returns(const char* buffer, size_t size);
    int64_t write_add_carriage_returns(cz::Str str) {
        return write_add_carriage_returns(str.buffer, str.len);
    }

    /// Wrapper for `write` and `write_add_carriage_returns` that
    /// selects the implementation based on the host operating system.
    int64_t write_text(const char* buffer, size_t size) {
#ifdef _WIN32
        return write_add_carriage_returns(buffer, size);
#else
        return write(buffer, size);
#endif
    }
    int64_t write_text(cz::Str str) {
        return write_text(str.buffer, str.len);
    }

    /// Flush writes to the file.
    bool flush();
};

int64_t write_loop(Output_File file, const char* buffer, size_t size);
inline int64_t write_loop(Output_File file, cz::Str str) {
    return write_loop(file, str.buffer, str.len);
}

Input_File std_in_file();
Output_File std_out_file();
Output_File std_err_file();

bool read_to_string(Input_File file, cz::Allocator allocator, cz::String* string);
bool read_to_string(const char* path, cz::Allocator allocator, cz::String* string);

////////////////////////////////////////////////////////////////////////////////
// Inline implementations
////////////////////////////////////////////////////////////////////////////////

inline static void strip_carriage_returns(String* string) {
    return strip_carriage_returns(string->buffer, &string->len);
}
inline static void strip_trailing_carriage_return(String* string, Carriage_Return_Carry* carry) {
    return strip_trailing_carriage_return(string->buffer, &string->len, carry);
}
inline static void strip_carriage_returns(char* buffer,
                                          size_t* size,
                                          Carriage_Return_Carry* carry) {
    strip_carriage_returns(buffer, size);
    strip_trailing_carriage_return(buffer, size, carry);
}
inline static void strip_carriage_returns(cz::String* string, Carriage_Return_Carry* carry) {
    return strip_carriage_returns(string->buffer, &string->len, carry);
}

}
