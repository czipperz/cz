#include <cz/logger.hpp>

#include <cz/assert.hpp>
#include <cz/mem.hpp>

namespace cz {
namespace log {

LogInfo::LogInfo(const char* file, size_t line, LogLevel level)
    : file(file), line(line), level(level) {}

Logger ignore() {
    static const Logger::VTable vtable = {
        [](void*, const LogInfo&) { return Result::ok(); },
        [](void*, const LogInfo&, Str) { return Result::ok(); },
        [](void*, const LogInfo&) { return Result::ok(); }};
    return {&vtable, nullptr};
}

static Result log_writer_write_str(void* data, Str str) {
    auto log_writer = static_cast<LogWriter*>(data);
    return log_writer->logger.write_chunk(log_writer->info, str);
}

Writer LogWriter::writer() {
    return {log_writer_write_str, this};
}

}

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
