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
#include <sys/wait.h>
#include <unistd.h>
#endif

#include <stdlib.h>
#include <cz/defer.hpp>
#include <cz/heap.hpp>

namespace cz {

void File_Descriptor::close() {
#ifdef _WIN32
    if (handle != Null_) {
        CloseHandle(handle);
    }
#else
    if (fd != Null_) {
        ::close(fd);
    }
#endif
}

bool Input_File::open(const char* file) {
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
    fd = ::open(file, O_RDONLY);
    return fd != -1;
#endif
}

bool File_Descriptor::set_non_blocking() {
#ifdef _WIN32
    DWORD mode = PIPE_NOWAIT;
    return SetNamedPipeHandleState(handle, &mode, NULL, NULL);
#else
    int res = fcntl(fd, F_GETFL);
    if (res < 0) {
        return false;
    }
    if (fcntl(fd, F_SETFL, res | O_NONBLOCK) < 0) {
        return false;
    }
    return true;
#endif
}

bool File_Descriptor::set_non_inheritable() {
#ifdef _WIN32
    return SetHandleInformation(handle, HANDLE_FLAG_INHERIT, FALSE);
#else
    int res = fcntl(fd, F_GETFD);
    if (res < 0) {
        return false;
    }
    if (fcntl(fd, F_SETFD, res | O_CLOEXEC) < 0) {
        return false;
    }
    return true;
#endif
}

int64_t Input_File::read_binary(char* buffer, size_t size) {
#ifdef _WIN32
    DWORD bytes;
    if (ReadFile(handle, buffer, size, &bytes, NULL)) {
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
    return ::read(fd, buffer, size);
#endif
}

void strip_carriage_returns(char* buffer, size_t* size) {
    char* start = buffer + 1;
    while (start - buffer < *size) {
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

int64_t Input_File::read_strip_carriage_returns(char* buffer,
                                                size_t size,
                                                Carriage_Return_Carry* carry) {
    char* start = buffer;
    char* end = buffer + size;
    if (carry->carrying && size > 0) {
        *start = '\r';
        ++start;
    }

    int64_t res = read_binary(start, end - start);
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
    fd = ::open(file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    return fd != -1;
#endif
}

int64_t Output_File::write_binary(const char* buffer, size_t size) {
#ifdef _WIN32
    DWORD bytes;
    if (WriteFile(handle, buffer, size, &bytes, NULL)) {
        return bytes;
    } else {
        return -1;
    }
#else
    return ::write(fd, buffer, size);
#endif
}

int64_t Output_File::write_add_carriage_returns(const char* buffer, size_t size) {
    const char* start = buffer;
    const char* end = buffer + size;
    while (1) {
        const char* spot = (const char*)memchr(start, '\n', end - start);
        if (spot) {
            int64_t res = write_binary(start, spot - start);
            if (res < 0) {
                return -1;
            } else if (res < spot - start) {
                return spot + res - buffer;
            }

            int64_t tres2 = write_binary("\r\n", 2);
            if (tres2 < 0) {
                return -1;
            } else if (tres2 < 2) {
                return spot + res - buffer;
            }

            start = spot + 1;
        } else {
            int64_t res = write_binary(start, end - start);
            if (res < 0) {
                return -1;
            }
            return start + res - buffer;
        }
    }
}

Input_File std_in_file() {
    Input_File file;
#ifdef _WIN32
    file.handle = GetStdHandle(STD_INPUT_HANDLE);
#else
    file.fd = 0;
#endif
    return file;
}

Output_File std_out_file() {
    Output_File file;
#ifdef _WIN32
    file.handle = GetStdHandle(STD_OUTPUT_HANDLE);
#else
    file.fd = 1;
#endif
    return file;
}

Output_File std_err_file() {
    Output_File file;
#ifdef _WIN32
    file.handle = GetStdHandle(STD_ERROR_HANDLE);
#else
    file.fd = 2;
#endif
    return file;
}

void read_to_string(Input_File file, cz::Allocator allocator, cz::String* out) {
    char buffer[1024];
    Carriage_Return_Carry carry;
    while (1) {
        int64_t read_result = file.read_text(buffer, sizeof(buffer), &carry);
        if (read_result < 0) {
            // Todo: what do we do here?  I'm just ignoring the error for now
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
    std_in.close();
    std_out.close();
#ifdef _WIN32
    if (std_out.handle != std_err.handle)
#else
    if (std_out.fd != std_err.fd)
#endif
    {
        std_err.close();
    }
}

bool create_pipe(Input_File* input, Output_File* output) {
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

    input->fd = fds[0];
    output->fd = fds[1];
    return true;
#endif
}

bool create_process_input_pipe(Input_File* input, Output_File* output) {
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
#ifdef _WIN32
    CloseHandle(hProcess);
#endif
}

void Process::kill() {
#ifdef _WIN32
    TerminateProcess(hProcess, -1);
#else
    ::kill(pid, SIGTERM);
#endif
    detach();
}

int Process::join() {
#ifdef _WIN32
    WaitForSingleObject(hProcess, INFINITE);
    DWORD exit_code = -1;
    GetExitCodeProcess(hProcess, &exit_code);
#else
    int status;
    waitpid(pid, &status, 0);
    int exit_code;
    if (WIFEXITED(status)) {
        exit_code = WEXITSTATUS(status);
    } else {
        exit_code = 127;
    }
#endif

    detach();
    return exit_code;
}

#ifdef _WIN32
static bool launch_script_(char* script, Process_Options* options, HANDLE* hProcess) {
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdError = options->std_err.handle;
    si.hStdOutput = options->std_out.handle;
    si.hStdInput = options->std_in.handle;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    DWORD creation_flags = 0;
    if (options->detach) {
        creation_flags |= DETACHED_PROCESS;
    }

    if (!CreateProcessA(nullptr, script, nullptr, nullptr, TRUE, creation_flags, nullptr,
                        options->working_directory, &si, &pi)) {
        return false;
    }

    *hProcess = pi.hProcess;
    CloseHandle(pi.hThread);
    return true;
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
    bool any_special = false;
    for (size_t i = 0; i < arg.len; ++i) {
        if (!isalnum(arg[i]) && arg[i] != '/' && arg[i] != ':' && arg[i] != '-') {
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

bool Process::launch_script(cz::Str script, Process_Options* options) {
    cz::Str prefix = "cmd /C ";

    char* copy = (char*)malloc(prefix.len + script.len + 1);
    if (!copy) {
        return false;
    }
    CZ_DEFER(free(copy));

    memcpy(copy, prefix.buffer, prefix.len);
    memcpy(copy + prefix.len, script.buffer, script.len);
    copy[prefix.len + script.len] = '\0';

    return launch_script_(copy, options, &hProcess);
}

bool Process::launch_program(cz::Slice<const cz::Str> args, Process_Options* options) {
    cz::String script = {};
    CZ_DEFER(script.drop(cz::heap_allocator()));

    escape_args(args, &script, cz::heap_allocator());

    return launch_script_(script.buffer(), options, &hProcess);
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

bool Process::launch_script(cz::Str script, Process_Options* options) {
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

bool Process::launch_program(cz::Slice<const cz::Str> args, Process_Options* options) {
    char** new_args = (char**)malloc(sizeof(char*) * (args.len + 1));
    CZ_ASSERT(new_args);
    CZ_DEFER(free(new_args));
    for (size_t i = 0; i < args.len; ++i) {
        cz::Str arg = args[i];

        char* new_arg = (char*)malloc(arg.len + 1);
        CZ_ASSERT(new_arg);
        memcpy(new_arg, arg.buffer, arg.len);
        new_arg[arg.len] = '\0';

        new_args[i] = new_arg;
    }
    new_args[args.len] = nullptr;
    CZ_DEFER(for (size_t i = 0; i < args.len; ++i) { free(new_args[i]); });

    pid = fork();
    if (pid < 0) {
        return false;
    } else if (pid == 0) {  // child process
        bind_pipe(options->std_in.fd, 0);
        bind_pipe(options->std_out.fd, 1);
        bind_pipe(options->std_err.fd, 2);

        close(options->std_in.fd);
        close(options->std_out.fd);
        if (options->std_err.fd != options->std_out.fd) {
            close(options->std_err.fd);
        }

        if (options->working_directory) {
            chdir(options->working_directory);
        }

        if (options->detach) {
            setsid();
        }

        // Launch the script by running it through the shell.
        execvp(new_args[0], new_args);

        // If exec returns there is an error launching.
        const char* message = "Error executing ";
        write(2, message, strlen(message));
        write(2, new_args[0], strlen(new_args[0]));
        write(2, ": ", 2);
        const char* err = strerror(errno);
        write(2, err, strlen(err));
        exit(errno);
    } else {  // parent process
        return true;
    }
}
#endif

void Process::escape_args(cz::Slice<const cz::Str> args,
                          cz::String* script,
                          cz::Allocator allocator) {
    CZ_DEBUG_ASSERT(args.len >= 1);

    script->reserve(cz::heap_allocator(), 32);

    for (size_t i = 0; i < args.len; ++i) {
        escape_arg(args[i], script, allocator);
        script->push(' ');
    }

    script->pop();
    script->null_terminate();
}

}
