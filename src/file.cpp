#include <cz/file.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace cz {
namespace file {

bool does_file_exist(const char* path) {
#ifdef _WIN32
    return GetFileAttributes(path) != 0xFFFFFFFF;
#else
    return access(path, F_OK) == 0;
#endif
}

bool is_directory(const char* path) {
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

}
}
