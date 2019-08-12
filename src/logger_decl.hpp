#pragma once

#include "context_decl.hpp"
#include "io/write.hpp"
#include "str.hpp"

namespace cz {
namespace log {

namespace LogLevel_ {
enum LogLevel {
    Off = -1,
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

struct LogInfo {
    const char* file;
    size_t line;
    LogLevel level;
    Str message;

    explicit LogInfo(const char* file, size_t line, LogLevel level, Str message);
};

struct LogFormatter {
    void (*impl)(C* c, void* data, io::Writer writer, LogInfo info);
    void* data;

    void write_to(C* c, io::Writer writer, LogInfo info) { return impl(c, data, writer, info); }
};

struct Logger {
    void (*impl)(C* c, void* data, LogInfo info);
    void* data;

    void log(C* c, LogInfo info) const { return impl(c, data, info); }
};

}
}
