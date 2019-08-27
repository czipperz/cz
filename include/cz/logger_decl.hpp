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

    LogInfo(const char* file, size_t line, LogLevel level);
};

struct Logger {
    struct VTable {
        io::Result (*write_prefix)(void* data, const LogInfo& info);
        io::Result (*write_chunk)(void* data, const LogInfo& info, Str chunk);
        io::Result (*write_suffix)(void* data, const LogInfo& info);
    };

    const VTable* vtable;
    void* data;

    io::Result write_prefix(const LogInfo& info) const { return vtable->write_prefix(data, info); }
    io::Result write_chunk(const LogInfo& info, Str chunk) const {
        return vtable->write_chunk(data, info, chunk);
    }
    io::Result write_suffix(const LogInfo& info) const { return vtable->write_suffix(data, info); }
};

}

namespace io {
Result write(Writer, log::LogLevel);
}
}
