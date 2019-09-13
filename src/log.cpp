#include <cz/log.hpp>

#include <cz/assert.hpp>

namespace cz {

LogInfo::LogInfo(const char* file, size_t line, LogLevel level)
    : file(file), line(line), level(level) {}

static Result log_writer_write_str(void* data, Str str) {
    auto log_writer = static_cast<LogWriter*>(data);
    return log_writer->logger.write_chunk(log_writer->info, str);
}

Writer LogWriter::writer() {
    return {log_writer_write_str, this};
}

Result write(Writer writer, LogLevel level) {
    switch (level) {
        case LogLevel::Fatal:
            return write(writer, "Fatal");
        case LogLevel::Error:
            return write(writer, "Error");
        case LogLevel::Warning:
            return write(writer, "Warning");
        case LogLevel::Important:
            return write(writer, "Important");
        case LogLevel::Information:
            return write(writer, "Information");
        case LogLevel::Debug:
            return write(writer, "Debug");
        case LogLevel::Trace:
            return write(writer, "Trace");
        default:
            CZ_PANIC("write(): Invalid log level");
    }
}

}
