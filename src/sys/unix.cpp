#include <cz/sys.hpp>

#ifndef _WIN32

#include <unistd.h>

namespace cz {
namespace sys {

size_t page_size() {
    return sysconf(_SC_PAGESIZE);
}

}
}

#endif
