#include "logger.hpp"

#include "assert.hpp"
#include "io.hpp"
#include "mem.hpp"

namespace cz {
namespace log {

LogInfo::LogInfo(const char* file, size_t line, LogLevel level, Str message)
    : file(file), line(line), level(level), message(message) {}

Logger ignore() {
    return {[](void*, LogInfo) {}, NULL};
}

}
}
