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

struct Logger {
    void (*target)(C* c, MemSlice data, LogLevel level, Str str);
    MemSlice data;

    void log(C* c, LogLevel level, Str str) { return target(c, data, level, str); }
};

io::Writer fatal();
io::Writer error();
io::Writer warning();
io::Writer important();
io::Writer information();
io::Writer debug();
io::Writer trace();

Logger ignore();
Logger console();

}
}
