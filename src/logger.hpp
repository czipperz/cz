#pragma once

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

struct Target {
    void (*log)(C* c, void* data, LogLevel level, Str str);
};

struct Logger {
    Target target;
    void* data;

    void log(C* c, LogLevel level, Str str) { return target.log(c, data, level, str); }
};

io::Writer fatal();
io::Writer error();
io::Writer warning();
io::Writer important();
io::Writer information();
io::Writer debug();
io::Writer trace();

Target ignore_target();
Target console_target(LogLevel max_log_level = LogLevel::Information);

}
}
