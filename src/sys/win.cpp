#include <cz/sys.hpp>

#ifdef _WIN32

#include <windows.h>

namespace cz {
namespace sys {

size_t page_size() {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwAllocationGranularity;
}

}
}

#endif
