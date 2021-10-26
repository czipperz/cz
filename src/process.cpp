#include <cz/process.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
extern char** environ;
#endif

#include <stdlib.h>
#include <cz/debug.hpp>
#include <cz/defer.hpp>
#include <cz/heap.hpp>

#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#else
#define ZoneScoped
#define ZoneText(...)
#define ZoneValue(...)
#endif

namespace cz {

void File_Descriptor::close() {
    ZoneScoped;

    if (is_open()) {
#ifdef _WIN32
        CloseHandle(handle);
#else
        ::close(handle);
#endif
    }
}

bool File_Descriptor::is_open() const {
    return handle != Null_;
}

bool Input_File::open(const char* file) {
    ZoneScoped;
    ZoneText(file, strlen(file));

#ifdef _WIN32
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    void* h = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, &sa, OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        return false;
    }
    handle = h;
    return true;
#else
    handle = ::open(file, O_RDONLY);
    return handle != -1;
#endif
}

bool File_Descriptor::set_non_blocking() {
    ZoneScoped;
    CZ_DEBUG_ASSERT(is_open());

#ifdef _WIN32
    DWORD mode = PIPE_NOWAIT;
    return SetNamedPipeHandleState(handle, &mode, NULL, NULL);
#else
    int res = fcntl(handle, F_GETFL);
    if (res < 0) {
        return false;
    }
    if (fcntl(handle, F_SETFL, res | O_NONBLOCK) < 0) {
        return false;
    }
    return true;
#endif
}

bool File_Descriptor::set_non_inheritable() {
    ZoneScoped;
    CZ_DEBUG_ASSERT(is_open());

#ifdef _WIN32
    return SetHandleInformation(handle, HANDLE_FLAG_INHERIT, FALSE);
#else
    int res = fcntl(handle, F_GETFD);
    if (res < 0) {
        return false;
    }
    if (fcntl(handle, F_SETFD, res | FD_CLOEXEC) < 0) {
        return false;
    }
    return true;
#endif
}

int64_t File_Descriptor::set_position(int64_t value, Relative_To relative_to) {
    ZoneScoped;
    CZ_DEBUG_ASSERT(is_open());

#ifdef _WIN32
    DWORD move_method;
    switch (relative_to) {
        case Relative_To::START:
            move_method = FILE_BEGIN;
            break;
        case Relative_To::END:
            move_method = FILE_END;
            break;
        case Relative_To::CURRENT:
            move_method = FILE_CURRENT;
            break;
    }

    LARGE_INTEGER large_integer;
    large_integer.QuadPart = (LONGLONG)value;
    if (!SetFilePointerEx(handle, large_integer, &large_integer, move_method)) {
        return -1;
    }
    return (int64_t)large_integer.QuadPart;
#else
    int whence;
    switch (relative_to) {
        case Relative_To::START:
            whence = SEEK_SET;
            break;
        case Relative_To::END:
            whence = SEEK_END;
            break;
        case Relative_To::CURRENT:
            whence = SEEK_CUR;
            break;
    }

    return lseek(handle, value, whence);
#endif
}

int64_t File_Descriptor::get_size() {
    ZoneScoped;
    CZ_DEBUG_ASSERT(is_open());

#ifdef _WIN32
    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(handle, &file_size)) {
        return -1;
    }
    return file_size.QuadPart;
#else
    struct stat buf;
    if (fstat(handle, &buf) < 0) {
        return -1;
    }
    return buf.st_size;
#endif
}

int64_t Input_File::read(void* buffer, size_t size) {
    ZoneScoped;
    ZoneValue(size);
    CZ_DEBUG_ASSERT(is_open());

#ifdef _WIN32
    DWORD bytes;
    if (ReadFile(handle, buffer, (DWORD)size, &bytes, NULL)) {
        return bytes;
    } else {
        // If we're reading non-blocking then no data also has
        // bytes == 0 but needs to be considered an error.
        DWORD error = GetLastError();
        if (error == ERROR_NO_DATA) {
            return -1;
        } else if (bytes == 0 /* eof */) {
            return bytes;
        } else {
            return -1;
        }
    }
#else
    return ::read(handle, buffer, size);
#endif
}

void strip_carriage_returns(char* buffer, size_t* size) {
    ZoneScoped;

    char* start = buffer + 1;
    while (start < buffer + *size) {
        char* spot = (char*)memchr(start, '\n', buffer + *size - start);
        if (spot) {
            if (spot[-1] == '\r') {
                memmove(spot - 1, spot, buffer + *size - spot);
                --*size;
                start = spot + 1;
            } else {
                start = spot + 2;
            }
        } else {
            break;
        }
    }
}

void strip_carriage_returns(String* string) {
    strip_carriage_returns(string->buffer, &string->len);
}

int64_t Input_File::read_strip_carriage_returns(char* buffer,
                                                size_t size,
                                                Carriage_Return_Carry* carry) {
    ZoneScoped;
    CZ_DEBUG_ASSERT(is_open());

    char* start = buffer;
    char* end = buffer + size;
    if (carry->carrying && size > 0) {
        *start = '\r';
        ++start;
    }

    int64_t res = read(start, end - start);
    if (res < 0) {
        return -1;
    } else {
        size = res + start - buffer;
        end = buffer + size;
    }

    strip_carriage_returns(buffer, &size);

    if (size > 0 && buffer[size - 1] == '\r') {
        carry->carrying = true;
        return size - 1;
    } else {
        carry->carrying = false;
        return size;
    }
}

bool Output_File::open(const char* file) {
    ZoneScoped;
    ZoneText(file, strlen(file));

#ifdef _WIN32
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    void* h = CreateFile(file, GENERIC_WRITE, 0, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        return false;
    }
    handle = h;
    return true;
#else
    handle = ::open(file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    return handle != -1;
#endif
}

int64_t Output_File::write(const void* buffer, size_t size) {
    ZoneScoped;
    ZoneValue(size);
    CZ_DEBUG_ASSERT(is_open());

#ifdef _WIN32
    DWORD bytes;
    if (WriteFile(handle, buffer, (DWORD)size, &bytes, NULL)) {
        if (bytes == 0)
            return -1;  // No space left.
        return bytes;
    } else {
        return -1;
    }
#else
    return ::write(handle, buffer, size);
#endif
}

int64_t Output_File::write_add_carriage_returns(const char* buffer, size_t size) {
    ZoneScoped;
    CZ_DEBUG_ASSERT(is_open());

    const char* start = buffer;
    const char* end = buffer + size;
    while (1) {
        const char* spot = (const char*)memchr(start, '\n', end - start);
        if (spot) {
            int64_t res = write(start, spot - start);
            if (res < 0) {
                return -1;
            } else if (res < spot - start) {
                return start + res - buffer;
            }

            int64_t tres2 = write("\r\n", 2);
            if (tres2 < 0) {
                return -1;
            } else if (tres2 < 2) {
                return spot - buffer;
            }

            start = spot + 1;
        } else {
            int64_t res = write(start, end - start);
            if (res < 0) {
                return -1;
            }
            return start + res - buffer;
        }
    }
}

bool Output_File::flush() {
    ZoneScoped;
    CZ_DEBUG_ASSERT(is_open());

#ifdef _WIN32
    return FlushFileBuffers(handle);
#else
    return fsync(handle) == 0;
#endif
}

int64_t write_loop(Output_File file, const char* buffer, size_t size) {
    size_t written = 0;
    while (written < size) {
        int64_t result = file.write(buffer + written, size - written);
        if (result > 0) {
            written += result;
        } else if (result == 0) {
            break;
        } else {
            if (written == 0) {
                return result;
            }
            break;
        }
    }
    return written;
}

Input_File std_in_file() {
    Input_File file;
#ifdef _WIN32
    file.handle = GetStdHandle(STD_INPUT_HANDLE);
#else
    file.handle = 0;
#endif
    return file;
}

Output_File std_out_file() {
    Output_File file;
#ifdef _WIN32
    file.handle = GetStdHandle(STD_OUTPUT_HANDLE);
#else
    file.handle = 1;
#endif
    return file;
}

Output_File std_err_file() {
    Output_File file;
#ifdef _WIN32
    file.handle = GetStdHandle(STD_ERROR_HANDLE);
#else
    file.handle = 2;
#endif
    return file;
}

void read_to_string(Input_File file, cz::Allocator allocator, cz::String* out) {
    ZoneScoped;
    CZ_DEBUG_ASSERT(file.is_open());

    char buffer[1024];
    Carriage_Return_Carry carry;
    while (1) {
        int64_t read_result = file.read_text(buffer, sizeof(buffer), &carry);
        if (read_result < 0) {
            // TODO: what do we do here?  I'm just ignoring the error for now
            cz::dbreak();
        } else if (read_result == 0) {
            // End of file
            break;
        } else {
            out->reserve(allocator, read_result);
            out->append({buffer, (size_t)read_result});
        }
    }
}

void Process_Options::close_all() {
    ZoneScoped;

    std_in.close();
    std_out.close();
    if (std_out.handle != std_err.handle) {
        std_err.close();
    }
}

bool create_pipe(Input_File* input, Output_File* output) {
    ZoneScoped;

#ifdef _WIN32
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    return CreatePipe(&input->handle, &output->handle, &sa, 0);
#else
    int fds[2];
    if (pipe(fds) < 0) {
        return false;
    }

    input->handle = fds[0];
    output->handle = fds[1];
    return true;
#endif
}

bool create_process_input_pipe(Input_File* input, Output_File* output) {
    ZoneScoped;

    if (!create_pipe(input, output)) {
        return false;
    }

    if (!output->set_non_inheritable()) {
        input->close();
        output->close();
        return false;
    }

    return true;
}

bool create_process_output_pipe(Output_File* output, Input_File* input) {
    ZoneScoped;

    if (!create_pipe(input, output)) {
        return false;
    }

    if (!input->set_non_inheritable()) {
        input->close();
        output->close();
        return false;
    }

    return true;
}

bool create_process_pipes(Process_IO* io, Process_Options* options) {
    ZoneScoped;

    if (!create_process_input_pipe(&options->std_in, &io->std_in)) {
        return false;
    }

    if (!create_process_output_pipe(&options->std_out, &io->std_out)) {
        options->std_in.close();
        io->std_in.close();
        return false;
    }

    options->std_err = options->std_out;

    return true;
}

bool create_process_pipes(Process_IOE* io, Process_Options* options) {
    ZoneScoped;

    if (!create_process_input_pipe(&options->std_in, &io->std_in)) {
        return false;
    }

    if (!create_process_output_pipe(&options->std_out, &io->std_out)) {
        options->std_in.close();
        io->std_in.close();
        return false;
    }

    if (!create_process_output_pipe(&options->std_err, &io->std_err)) {
        options->std_out.close();
        io->std_out.close();
        options->std_in.close();
        io->std_in.close();
        return false;
    }

    return true;
}

void Process::detach() {
    ZoneScoped;

#ifdef _WIN32
    CloseHandle(hProcess);
#endif
}

void Process::kill() {
    ZoneScoped;

#ifdef _WIN32
    TerminateProcess(hProcess, -1);
    CloseHandle(hProcess);
#else
    ::kill(pid, SIGTERM);
#endif
}

#ifdef _WIN32
static int get_exit_code(HANDLE hProcess) {
    DWORD exit_code = -1;
    GetExitCodeProcess(hProcess, &exit_code);
    return exit_code;
}
#else
static int get_exit_code(int status) {
    int exit_code;
    if (WIFEXITED(status)) {
        exit_code = WEXITSTATUS(status);
    } else {
        exit_code = 127;
    }
    return exit_code;
}
#endif

int Process::join() {
    ZoneScoped;

#ifdef _WIN32
    DWORD status = WaitForSingleObject(hProcess, INFINITE);
    CZ_ASSERT(status == WAIT_OBJECT_0);

    int exit_code = get_exit_code(hProcess);
    CloseHandle(hProcess);
#else
    int status;
    waitpid(pid, &status, 0);

    int exit_code = get_exit_code(status);
#endif

    return exit_code;
}

bool Process::try_join(int* exit_code) {
    ZoneScoped;

#ifdef _WIN32
    DWORD status = WaitForSingleObject(hProcess, 0);
    if (status != WAIT_OBJECT_0) {
        return false;
    }

    *exit_code = get_exit_code(hProcess);
    CloseHandle(hProcess);
#else
    int status;
    if (waitpid(pid, &status, WNOHANG) == 0) {
        return false;
    }

    *exit_code = get_exit_code(status);
#endif

    return true;
}

#ifdef _WIN32
static LPPROC_THREAD_ATTRIBUTE_LIST make_pseudo_console_attribute_list(HPCON pseudo_console);
static void cleanup_pseudo_console_attribute_list(LPPROC_THREAD_ATTRIBUTE_LIST attributes);

static bool launch_script_(char* script, const Process_Options& options, HANDLE* hProcess) {
    ZoneScoped;

    // Use STARTUPINFOEX to allow Pseudo Console support if we need it.
    STARTUPINFOEX si;
    ZeroMemory(&si, sizeof(si));
    if (options.pseudo_console) {
        si.StartupInfo.cb = sizeof(STARTUPINFOEX);
        si.lpAttributeList = make_pseudo_console_attribute_list((HPCON)options.pseudo_console);
        if (!si.lpAttributeList)
            return false;
    } else {
        si.StartupInfo.cb = sizeof(STARTUPINFO);
        si.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
        si.StartupInfo.hStdError = options.std_err.handle;
        si.StartupInfo.hStdOutput = options.std_out.handle;
        si.StartupInfo.hStdInput = options.std_in.handle;
    }

    CZ_DEFER(cleanup_pseudo_console_attribute_list(si.lpAttributeList));

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    DWORD creation_flags = 0;
    if (options.detach) {
        creation_flags |= DETACHED_PROCESS;
    }
    if (options.pseudo_console) {
        creation_flags |= EXTENDED_STARTUPINFO_PRESENT;
    }
    creation_flags |= CREATE_NO_WINDOW;

    if (!CreateProcessA(nullptr, script, nullptr, nullptr, true, creation_flags,
                        options.environment, options.working_directory, &si.StartupInfo, &pi)) {
        return false;
    }

    *hProcess = pi.hProcess;
    CloseHandle(pi.hThread);
    return true;
}

static LPPROC_THREAD_ATTRIBUTE_LIST make_pseudo_console_attribute_list(HPCON pseudo_console) {
    // The size isn't static so we have to query the size...
    size_t size = 0;
    // Note: intentionally disregard errors, they are expected.
    InitializeProcThreadAttributeList(nullptr, 1, 0, &size);

    // Then dynamically allocate...
    LPPROC_THREAD_ATTRIBUTE_LIST attributes =
        (LPPROC_THREAD_ATTRIBUTE_LIST)cz::heap_allocator().alloc({size, alignof(max_align_t)});
    if (!attributes)
        goto err_alloc;

    // And finally initialize our struct.
    if (!InitializeProcThreadAttributeList(attributes, 1, 0, &size))
        goto err_init;

    // Tell the child process that is in inside a pseudo console.
    if (!UpdateProcThreadAttribute(attributes, 0, PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
                                   pseudo_console, sizeof(HPCON), nullptr, nullptr))
        goto err_update;

    return attributes;

err_update:
    DeleteProcThreadAttributeList(attributes);
err_init:
    cz::heap_allocator().dealloc({attributes, size});
err_alloc:
    return nullptr;
}

static void cleanup_pseudo_console_attribute_list(LPPROC_THREAD_ATTRIBUTE_LIST attributes) {
    if (attributes) {
        DeleteProcThreadAttributeList(attributes);
        cz::heap_allocator().dealloc({attributes, 0});
    }
}

static void escape_backslashes(cz::String* script, cz::Str arg, size_t i) {
    for (size_t j = i; j-- > 0;) {
        if (arg[j] == '\\') {
            script->reserve(cz::heap_allocator(), 1);
            script->push('\\');
        } else {
            break;
        }
    }
}

void Process::escape_arg(cz::Str arg, cz::String* script, cz::Allocator allocator) {
    ZoneScoped;

    bool any_special = false;
    for (size_t i = 0; i < arg.len; ++i) {
        if (!isalnum(arg[i]) && arg[i] != '/' && arg[i] != '\\' && arg[i] != ':' && arg[i] != '-' &&
            arg[i] != '.') {
            any_special = true;
            break;
        }
    }

    if (any_special) {
        script->reserve(allocator, 3 + arg.len);
        script->push('"');

        for (size_t i = 0; i < arg.len; ++i) {
            if (arg[i] == '"') {
                escape_backslashes(script, arg, i);

                script->reserve(allocator, 2);
                script->push('\\');
            } else if (arg[i] == '%' && i + 1 < arg.len) {
                if (i == 0 || arg[i - 1] == '%') {
                    script->reserve(allocator, 1);
                    script->pop();
                } else {
                    script->reserve(allocator, 3);
                    script->push('"');
                }
                script->push('%');
                script->push('"');
                continue;
            }

            script->reserve(allocator, 1);
            script->push(arg[i]);
        }

        escape_backslashes(script, arg, arg.len);

        if (arg.last() == '%' && script->last() == '"') {
            script->reserve(allocator, 1);
        } else {
            script->reserve(allocator, 2);
            script->push('"');
        }
    } else {
        script->reserve(allocator, arg.len + 1);
        script->append(arg);
    }
}

bool Process::launch_script(cz::Str script, const Process_Options& options) {
    ZoneScoped;

    cz::Str prefix = "cmd /C ";

    cz::String copy = {};
    CZ_DEFER(copy.drop(cz::heap_allocator()));
    copy.reserve(cz::heap_allocator(), prefix.len + script.len + 1);

    copy.append(prefix);
    copy.append(script);
    copy.null_terminate();

    return launch_script_(copy.buffer, options, &hProcess);
}

bool Process::launch_program(cz::Slice<const cz::Str> args, const Process_Options& options) {
    ZoneScoped;

    cz::String script = {};
    CZ_DEFER(script.drop(cz::heap_allocator()));

    escape_args(args, &script, cz::heap_allocator());

    return launch_script_(script.buffer, options, &hProcess);
}

#else

static bool shell_escape_inside(char c) {
    switch (c) {
        case '!':
        case '"':
        case '$':
        case '\\':
        case '`':
            return true;

        default:
            return false;
    }
}

static bool shell_escape_outside(char c) {
    switch (c) {
        case ' ':
        case '!':
        case '"':
        case '#':
        case '$':
        case '&':
        case '\'':
        case '(':
        case ')':
        case '*':
        case ',':
        case ';':
        case '<':
        case '>':
        case '?':
        case '[':
        case '\\':
        case ']':
        case '^':
        case '`':
        case '{':
        case '|':
        case '}':
            return true;

        default:
            return false;
    }
}

void Process::escape_arg(cz::Str arg, cz::String* script, cz::Allocator allocator) {
    ZoneScoped;

    size_t escaped_outside = 0;
    size_t escaped_inside = 0;
    bool use_string = false;
    for (size_t i = 0; i < arg.len; ++i) {
        bool out = shell_escape_outside(arg[i]);
        bool in = shell_escape_inside(arg[i]);
        if (out) {
            escaped_outside++;
        }
        if (in) {
            escaped_inside++;
        }
        if (out && !in) {
            use_string = true;
        }
    }

    if (use_string) {
        script->reserve(allocator, 3 + arg.len + escaped_inside);
        script->push('"');

        for (size_t i = 0; i < arg.len; ++i) {
            if (shell_escape_inside(arg[i])) {
                script->push('\\');
            }

            script->push(arg[i]);
        }

        script->push('"');
    } else {
        script->reserve(allocator, 1 + arg.len + escaped_outside);

        for (size_t i = 0; i < arg.len; ++i) {
            if (shell_escape_outside(arg[i])) {
                script->push('\\');
            }

            script->push(arg[i]);
        }
    }
}

bool Process::launch_script(cz::Str script, const Process_Options& options) {
    ZoneScoped;

    cz::Str args[] = {"/bin/sh", "-c", script, nullptr};
    return launch_program(args, options);
}

static void bind_pipe(int input, int output) {
    if (input != File_Descriptor::Null_) {
        dup2(input, output);
    } else {
        close(output);
    }
}

bool Process::launch_program(cz::Slice<const cz::Str> args, const Process_Options& options) {
    ZoneScoped;

    char** new_args = cz::heap_allocator().alloc<char*>(args.len + 1);
    CZ_ASSERT(new_args);
    CZ_DEFER(cz::heap_allocator().dealloc(new_args, args.len + 1));

    for (size_t i = 0; i < args.len; ++i) {
        new_args[i] = args[i].clone_null_terminate(cz::heap_allocator()).buffer;
    }
    new_args[args.len] = nullptr;

    CZ_DEFER(for (size_t i = 0; i < args.len; ++i) {
        cz::heap_allocator().dealloc({new_args[i], args[i].len + 1});
    });

    pid = fork();
    if (pid < 0) {
        return false;
    } else if (pid == 0) {  // child process
        bind_pipe(options.std_in.handle, 0);
        bind_pipe(options.std_out.handle, 1);
        bind_pipe(options.std_err.handle, 2);

        if (options.std_in.handle != 0) {
            close(options.std_in.handle);
        }
        if (options.std_out.handle != 1) {
            close(options.std_out.handle);
        }
        if (options.std_err.handle != 2) {
            if (options.std_err.handle != options.std_out.handle) {
                close(options.std_err.handle);
            }
        }

        if (options.working_directory) {
            chdir(options.working_directory);
        }

        if (options.environment) {
            environ = options.environment;
        }

        if (options.detach) {
            setsid();
        }

        // Launch the script by running it through the shell.
        execvp(new_args[0], new_args);

        // If exec returns there is an error launching.
        const char* message = "Error executing ";
        (void)write(2, message, strlen(message));
        (void)write(2, new_args[0], strlen(new_args[0]));
        (void)write(2, ": ", 2);
        const char* err = strerror(errno);
        (void)write(2, err, strlen(err));
        exit(errno);
    } else {  // parent process
        return true;
    }
}
#endif

void Process::escape_args(cz::Slice<const cz::Str> args,
                          cz::String* script,
                          cz::Allocator allocator) {
    ZoneScoped;

    CZ_DEBUG_ASSERT(args.len >= 1);

    script->reserve(allocator, 32);

    for (size_t i = 0; i < args.len; ++i) {
        escape_arg(args[i], script, allocator);
        script->push(' ');
    }

    script->pop();
    script->null_terminate();
}

}
