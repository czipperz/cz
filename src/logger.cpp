#include <cz/logger.hpp>

#include <cz/assert.hpp>
#include <cz/io.hpp>
#include <cz/mem.hpp>

namespace cz {
namespace log {

LogInfo::LogInfo(const char* file, size_t line, LogLevel level, Str message)
    : file(file), line(line), level(level), message(message) {}

Logger ignore() {
    return {[](void*, LogInfo) {}, nullptr};
}

}

namespace io {

Result write(Writer writer, log::LogLevel level) {
    switch (level) {
        case log::LogLevel::Fatal:
            return write(writer, "Fatal");
        case log::LogLevel::Error:
            return write(writer, "Error");
        case log::LogLevel::Warning:
            return write(writer, "Warning");
        case log::LogLevel::Important:
            return write(writer, "Important");
        case log::LogLevel::Information:
            return write(writer, "Information");
        case log::LogLevel::Debug:
            return write(writer, "Debug");
        case log::LogLevel::Trace:
            return write(writer, "Trace");
        default:
            CZ_PANIC("write(): Invalid log level");
    }
}

}

}
