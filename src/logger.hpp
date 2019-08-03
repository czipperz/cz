#pragma once

#include "string.hpp"
#include "io/write.hpp"

namespace cz {
namespace log {

namespace LogLevel_ {
enum LogLevel {
    Fatal,
    Error,
    Warning,
    Important,
    Information,
    Debug,
    Trace,
};
}
using LogLevel_::LogLevel;

using Log = void (*)(void* data, LogLevel level, Str str);

struct Logger {
    Log impl;
    void* data;

    void log(LogLevel level, Str str) { return impl(data, level, str); }

    /// This uses thread local storage to create a writer.
    io::Writer writer_local(LogLevel level);

    io::Writer writer_alloc(LogLevel level);
    void writer_dealloc(io::Writer log_writer);
};

extern Logger global_logger;

inline io::Writer log(LogLevel level) {
    return global_logger.writer_local(level);
}
inline io::Writer fatal() {
    return log(LogLevel::Fatal);
}
inline io::Writer error() {
    return log(LogLevel::Error);
}
inline io::Writer warning() {
    return log(LogLevel::Warning);
}
inline io::Writer important() {
    return log(LogLevel::Important);
}
inline io::Writer information() {
    return log(LogLevel::Information);
}
inline io::Writer debug() {
    return log(LogLevel::Debug);
}
inline io::Writer trace() {
    return log(LogLevel::Trace);
}

}
}
