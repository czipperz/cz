#include <cz/file.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#else
#define ZoneScoped (void)0
#endif

namespace cz {
namespace file {

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

}
}
