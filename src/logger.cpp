#include "logger.hpp"

#include "assert.hpp"
#include "io.hpp"
#include "mem.hpp"

namespace cz {
namespace log {

LogInfo::LogInfo(LogLevel level, Str message) : level(level), message(message) {}

}
}
