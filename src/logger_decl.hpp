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
    LogLevel level;
    Str message;

    explicit LogInfo(LogLevel level, Str message);
};

struct LogFormatter {
    void (*target)(C* c, void* data, io::Writer writer, LogInfo info);
    void* data;

    void write_to(C* c, io::Writer writer, LogInfo info) { return target(c, data, writer, info); }
    void drop(C* c);
};

struct Logger {
    LogFormatter formatter;
    io::Writer out;

    void log(C* c, LogInfo info) { return formatter.write_to(c, out, info); }
};

}
}
