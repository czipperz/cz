#include <cz/file.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#else
#define ZoneScoped (void)0
#define ZoneText(...)
#define ZoneValue(...)
#endif

#include <stdio.h>
#include <cz/defer.hpp>

namespace cz {
namespace file {

///////////////////////////////////////////////////////////////////////////////
// Path tests
///////////////////////////////////////////////////////////////////////////////

bool exists(const char* path) {
    ZoneScoped;
#ifdef _WIN32
    return GetFileAttributes(path) != 0xFFFFFFFF;
#else
    return access(path, F_OK) == 0;
#endif
}

bool is_directory(const char* path) {
    ZoneScoped;
#ifdef _WIN32
    DWORD result = GetFileAttributes(path);
    if (result == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return result & FILE_ATTRIBUTE_DIRECTORY;
#else
    struct stat buf;
    if (stat(path, &buf) < 0) {
        return false;
    }
    return S_ISDIR(buf.st_mode);
#endif
}

bool is_directory_and_not_symlink(const char* path) {
    ZoneScoped;
#ifdef _WIN32
    DWORD result = GetFileAttributes(path);
    if (result == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (result & FILE_ATTRIBUTE_DIRECTORY) && !(result & FILE_ATTRIBUTE_REPARSE_POINT);
#else
    struct stat buf;
    if (lstat(path, &buf) < 0) {
        return false;
    }
    return S_ISDIR(buf.st_mode) && !S_ISLNK(buf.st_mode);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Path modifications
///////////////////////////////////////////////////////////////////////////////

int create_directory(const char* path) {
    ZoneScoped;
#ifdef _WIN32
    if (CreateDirectoryA(path, NULL)) {
        return 0;
    }

    int error = GetLastError();
    if (error == ERROR_ALREADY_EXISTS) {
        return 2;
    } else {
        return 1;
    }
#else
    if (mkdir(path, 0755) == 0) {
        return 0;
    }

    int error = errno;
    if (error == EEXIST) {
        return 2;
    } else {
        return 1;
    }
#endif
}

bool remove_empty_directory(const char* path) {
#ifdef _WIN32
    return RemoveDirectoryA(path);
#else
    return rmdir(path) == 0;
#endif
}

bool remove_file(const char* path) {
#ifdef _WIN32
    return DeleteFileA(path);
#else
    return unlink(path) == 0;
#endif
}

bool rename_file(const char* old_path, const char* new_path) {
    return rename(old_path, new_path) == 0;
}

}

///////////////////////////////////////////////////////////////////////////////
// File Descriptor methods
///////////////////////////////////////////////////////////////////////////////

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

bool File_Descriptor::set_blocking() {
    ZoneScoped;
    CZ_DEBUG_ASSERT(is_open());

#ifdef _WIN32
    DWORD mode = 0;
    return SetNamedPipeHandleState(handle, &mode, NULL, NULL);
#else
    int res = fcntl(handle, F_GETFL);
    if (res < 0) {
        return false;
    }
    if (fcntl(handle, F_SETFL, res & ~O_NONBLOCK) < 0) {
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

bool File_Descriptor::set_inheritable() {
    ZoneScoped;
    CZ_DEBUG_ASSERT(is_open());

#ifdef _WIN32
    return SetHandleInformation(handle, HANDLE_FLAG_INHERIT, TRUE);
#else
    int res = fcntl(handle, F_GETFD);
    if (res < 0) {
        return false;
    }
    if (fcntl(handle, F_SETFD, res & ~FD_CLOEXEC) < 0) {
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

///////////////////////////////////////////////////////////////////////////////
// Input File methods
///////////////////////////////////////////////////////////////////////////////

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

    strip_carriage_returns(buffer, &size, carry);
    return size;
}

void strip_carriage_returns(char* buffer, size_t* size) {
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

void strip_trailing_carriage_return(char* buffer, size_t* size, Carriage_Return_Carry* carry) {
    bool has_trailing_carriage_return = *size > 0 && buffer[*size - 1] == '\r';
    if (has_trailing_carriage_return) {
        --*size;
    }
    *carry->carrying = has_trailing_carriage_return;
}

///////////////////////////////////////////////////////////////////////////////
// Output File methods
///////////////////////////////////////////////////////////////////////////////

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
        if (bytes == 0 && size != 0)
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

///////////////////////////////////////////////////////////////////////////////
// Stdio files
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// Read to string
///////////////////////////////////////////////////////////////////////////////

bool read_to_string(Input_File file, cz::Allocator allocator, cz::String* out) {
    ZoneScoped;
    CZ_DEBUG_ASSERT(file.is_open());

    char buffer[1024];
    Carriage_Return_Carry carry;
    while (1) {
        int64_t read_result = file.read_text(buffer, sizeof(buffer), &carry);
        if (read_result < 0) {
            return false;
        } else if (read_result == 0) {
            // End of file
            break;
        } else {
            out->reserve(allocator, read_result);
            out->append({buffer, (size_t)read_result});
        }
    }
    return true;
}

bool read_to_string(const char* path, cz::Allocator allocator, cz::String* out) {
    Input_File file;
    if (!file.open(path))
        return false;

    CZ_DEFER(file.close());
    return read_to_string(file, allocator, out);
}

}
