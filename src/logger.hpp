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

using Log = void (*)(C* c, void* data, LogLevel level, Str str);

struct Logger {
    Log impl;
    void* data;

    void log(C* c, LogLevel level, Str str) { return impl(c, data, level, str); }
};

extern Logger global_logger;
extern LogLevel global_max_log_level;

io::Writer fatal();
io::Writer error();
io::Writer warning();
io::Writer important();
io::Writer information();
io::Writer debug();
io::Writer trace();

}
}
