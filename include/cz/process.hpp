#pragma once

#include <stdint.h>
#include <cz/allocator.hpp>
#include <cz/string.hpp>

#ifdef _WIN32
#else
#include <sys/types.h>
#endif

namespace cz {

struct File_Descriptor {
#ifdef _WIN32
    void* handle = Null_;

    static constexpr void* const Null_ = (void*)-1;
#else
    int fd = Null_;

    static constexpr const int Null_ = -1;
#endif

    void close();

    /// Set the file descriptor to return swiftly from IO operations.
    ///
    /// This is most useful when used on an `Input_File`.  In this case read will immediately
    /// return when nothing can be read rather than stalling.
    bool set_non_blocking();

    /// Set the file descriptor to be automatically closed when a process is created.
    bool set_non_inheritable();
};

struct Carriage_Return_Carry {
    bool carrying = false;
};

void strip_carriage_returns(char* buffer, size_t* size);

struct Input_File : File_Descriptor {
    /// Try to open a file for reading.
    ///
    /// Returns `true` if it succeeds, `false` otherwise.
    bool open(const char* file);

    /// Read up to `size` bytes from the file into `buffer`.
    ///
    /// Returns the number of bytes actually read, or `-1` on failure.  Returns `0` on end of file.
    int64_t read_binary(char* buffer, size_t size);

    /// Read up to `size` bytes from the file into `buffer`.
    ///
    /// Returns the number of bytes actually read, or `-1` on failure.  Does not count the number
    /// of carriage returns.  Returns `0` on end of file.
    ///
    /// This is the same as `read_binary` but it will strip carriage returns before newline
    /// characters.
    int64_t read_strip_carriage_returns(char* buffer, size_t size, Carriage_Return_Carry*);

    /// Read up to `size` bytes from the file into `buffer`.
    ///
    /// Returns the number of bytes actually read, or `-1` on failure.  If carriage returns are
    /// stripped, they are not counted for the return value.  Returns `0` on end of file.
    ///
    /// This is the same as `read_binary` but on Windows it will strip carriage returns.
    int64_t read_text(char* buffer, size_t size, Carriage_Return_Carry* carry) {
#ifdef _WIN32
        return read_strip_carriage_returns(buffer, size, carry);
#else
        (void)carry;  // remove unused parameter warnings
        return read_binary(buffer, size);
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
    /// Returns the number of bytes actually written, or `-1` on failure.  Returns `0` on end of
    /// file.
    int64_t write_binary(const char* buffer, size_t size);

    /// Write `size` bytes from `buffer` to the file.
    ///
    /// Returns the number of bytes actually written, or `-1` on failure.  Doesn't count carriage
    /// returns.  Returns `0` on end of file.
    ///
    /// This is the same as `write_binary` but adds carriage returns before each newline.
    int64_t write_add_carriage_returns(const char* buffer, size_t size);

    /// Write `size` bytes from `buffer` to the file.
    ///
    /// Returns the number of bytes actually written, or `-1` on failure.  If on Windows, doesn't
    /// count the extra carriage returns.  Returns `0` on end of file.
    ///
    /// This is the same as `write_binary` but on Windows it will add carriage returns.
    int64_t write_text(const char* buffer, size_t size) {
#ifdef _WIN32
        return write_add_carriage_returns(buffer, size);
#else
        return write_binary(buffer, size);
#endif
    }
};

Input_File std_in_file();
Output_File std_out_file();
Output_File std_err_file();

void read_to_string(Input_File, cz::Allocator allocator, cz::String* string);

struct Process_Options {
    Input_File std_in;
    Output_File std_out;
    Output_File std_err;

    /// The directory to run the process from.
    const char* working_directory = nullptr;

    /// Detach from the parent process instead of becoming a child process.
    bool detach = false;

    void close_all();
};

/// Create a pipe where both ends are inheritable.
bool create_pipe(Input_File*, Output_File*);

/// Create a pipe where the writing side is non-inheritable.  Use this for `Process_Options::stdin`.
bool create_process_input_pipe(Input_File*, Output_File*);
/// Create a pipe where the reading side is non-inheritable.  Use this for `Process_Options::stdout`
/// or `Process_Options::stderr`.
bool create_process_output_pipe(Output_File*, Input_File*);

struct Process_IOE {
    Output_File std_in;
    Input_File std_out;
    Input_File std_err;
};

struct Process_IO {
    Output_File std_in;
    Input_File std_out;
};

bool create_process_pipes(Process_IO*, Process_Options*);
bool create_process_pipes(Process_IOE*, Process_Options*);

struct Process {
private:
#ifdef _WIN32
    void* hProcess;
#else
    pid_t pid;
#endif

public:
    /// Escape one `arg` (argument) and append it to the `string`.
    ///
    /// Leaves an unallocated spot at the end of the `string` to either append a space (to split
    /// arguments) or a null terminator (to complete the script).
    static void escape_arg(cz::Str arg, cz::String* string, cz::Allocator allocator);

    /// Form a `script` from the list of `args` (arguments).
    ///
    /// Null terminates the script.
    static void escape_args(cz::Slice<const cz::Str> args,
                            cz::String* script,
                            cz::Allocator allocator);

    /// Launch a program.  The first argument must be the same the path to invoke.
    ///
    /// The process's `stdin`, `stdout`, and `stderr` streams are bound to the `options`' streams.
    /// The streams in `options` are not closed by this function.  Any files that are null (the
    /// default) are closed instead of being bound (in the new process).
    ///
    /// The return value is `true` if the program was successfully launched.
    bool launch_program(cz::Slice<const cz::Str> args, Process_Options* options);

    /// Launch a script as if it was ran on the command line.
    ///
    /// This runs the script through `cmd` on Windows and `/bin/sh` otherwise.
    ///
    /// See also `launch_program` for information on how `options` are handled.
    bool launch_script(cz::Str script, Process_Options* options);

    /// Kill the child process and destroy the `Process`.
    void kill();

    /// Wait for the child process to end if it hasn't ended already and destroy the `Process`.
    ///
    /// Returns the exit code of the process or a special code if the process terminated
    /// without exiting (crashed).  On Windows this is `-1`, on Linux this is `127`.
    int join();

    /// Try to join the process, returning whether it was successful.
    ///
    /// If it hasn't completed, returns `false`.  If it has completed then stores the
    /// exit code in `exit_code` (ie the return value of `join`) and returns `true`.
    ///
    /// See `join` for more information.
    bool try_join(int* exit_code);

    /// Destroy the `Process` but don't terminate the child process.
    void detach();
};

}
