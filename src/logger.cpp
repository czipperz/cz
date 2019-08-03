#include "logger.hpp"

#include <iostream>
#include "mem.hpp"

namespace cz {
namespace log {

struct LogWriter {
    Logger* logger;
    LogLevel level;
};

static void log_ignore(C*, void*, LogLevel, Str) {
    return;
}

Target ignore_target() {
    return {log_ignore};
}

static void log_console(C*, void*, LogLevel level, Str str) {
    FILE* stream;
    if (level <= LogLevel::Error) {
        stream = stderr;
    } else {
        stream = stdout;
    }

    fwrite(str.buffer, sizeof(char), str.len, stream);
}

Target console_target() {
    return {log_console};
}

template <LogLevel level>
static io::Result context_logger_write(C* c, void*, Str str) {
    if (level <= c->max_log_level) {
        c->logger.log(c, level, str);
    }
    return io::Result::ok();
}

#define define_log_writer(name, level) \
    io::Writer name() { return {{context_logger_write<LogLevel::level>}, NULL}; }

// clang-format off
define_log_writer(fatal, Fatal)
define_log_writer(error, Error)
define_log_writer(warning, Warning)
define_log_writer(important, Important)
define_log_writer(information, Information)
define_log_writer(debug, Debug)
define_log_writer(trace, Trace)
// clang-format on

}
}
