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

struct Logger {
    void (*impl)(void* data, LogInfo info);
    void* data;

    void log(LogInfo info) const { return impl(data, info); }
};

}
}
