#pragma once

#include "context_decl.hpp"
#include "write.hpp"
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
        Result (*write_prefix)(void* data, const LogInfo& info);
        Result (*write_chunk)(void* data, const LogInfo& info, Str chunk);
        Result (*write_suffix)(void* data, const LogInfo& info);
    };

    const VTable* vtable;
    void* data;

    Result write_prefix(const LogInfo& info) const { return vtable->write_prefix(data, info); }
    Result write_chunk(const LogInfo& info, Str chunk) const {
        return vtable->write_chunk(data, info, chunk);
    }
    Result write_suffix(const LogInfo& info) const { return vtable->write_suffix(data, info); }
};

}

Result write(Writer, log::LogLevel);

}
