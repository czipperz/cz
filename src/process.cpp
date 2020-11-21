#include <cz/process.hpp>

#ifdef _WIN32
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
    if (ReadFile(handle, buffer, size, &bytes, NULL) || bytes == 0 /* eof */) {
        return bytes;
    } else {
        return -1;
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

void Process::kill() {
#ifdef _WIN32
    TerminateProcess(hProcess, -1);
#else
    ::kill(pid, SIGTERM);
#endif
}

int Process::join() {
#ifdef _WIN32
    WaitForSingleObject(hProcess, INFINITE);
    DWORD exitCode = -1;
    GetExitCodeProcess(hProcess, &exitCode);
    return exitCode;
#else
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else {
        return 127;
    }
#endif
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

    if (!CreateProcessA(nullptr, script, nullptr, nullptr, TRUE, 0, nullptr,
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

static void add_argument(cz::String* script, cz::Str arg) {
    script->reserve(cz::heap_allocator(), 3 + arg.len);
    script->push('"');

    for (size_t i = 0; i < arg.len; ++i) {
        if (arg[i] == '"') {
            escape_backslashes(script, arg, i);

            script->reserve(cz::heap_allocator(), 2);
            script->push('\\');
        }

        script->reserve(cz::heap_allocator(), 1);
        script->push(arg[i]);
    }

    escape_backslashes(script, arg, arg.len);

    script->reserve(cz::heap_allocator(), 2);
    script->push('"');
    script->push(' ');
}

bool Process::launch_script(const char* script, Process_Options* options) {
    cz::Str prefix = "cmd /C ";
    size_t len = strlen(script);

    char* copy = (char*)malloc(prefix.len + len + 1);
    if (!copy) {
        return false;
    }
    CZ_DEFER(free(copy));

    memcpy(copy, prefix.buffer, prefix.len);
    memcpy(copy + prefix.len, script, len);
    copy[prefix.len + len] = 0;

    return launch_script_(copy, options, &hProcess);
}

bool Process::launch_program(const char* const* args, Process_Options* options) {
    cz::String script = {};
    script.reserve(cz::heap_allocator(), 32);
    CZ_DEFER(script.drop(cz::heap_allocator()));

    for (const char* const* arg = args; *arg; ++arg) {
        add_argument(&script, *arg);
    }
    script.reserve(cz::heap_allocator(), 1);
    script.null_terminate();

    return launch_script_(script.buffer(), options, &hProcess);
}

#else

bool Process::launch_script(const char* script, Process_Options* options) {
    const char* args[] = {"/bin/sh", "-c", script, nullptr};
    return launch_program(args, options);
}

static void bind_pipe(int input, int output) {
    if (input != File_Descriptor::Null_) {
        dup2(input, output);
    } else {
        close(output);
    }
}

bool Process::launch_program(const char* const* args, Process_Options* options) {
    size_t num_args = 0;
    while (args[num_args]) {
        ++num_args;
    }

    char** new_args = (char**)malloc(sizeof(char*) * (num_args + 1));
    CZ_ASSERT(new_args);
    CZ_DEFER(free(new_args));
    for (size_t i = 0; i < num_args; ++i) {
        const char* arg = args[i];
        size_t len = strlen(arg);

        char* new_arg = (char*)malloc(len + 1);
        CZ_ASSERT(new_arg);
        memcpy(new_arg, arg, len);
        new_arg[len] = 0;

        new_args[i] = new_arg;
    }
    new_args[num_args] = nullptr;
    CZ_DEFER(for (size_t i = 0; i < num_args; ++i) { free(new_args[i]); });

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

}
