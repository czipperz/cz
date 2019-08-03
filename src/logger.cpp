#include "logger.hpp"

#include <iostream>
#include "mem.hpp"

namespace cz {
namespace log {

struct LogWriter {
    Logger* logger;
    LogLevel level;
};

static void default_log(C*, void*, LogLevel level, Str str) {
    if (level > global_max_log_level) {
        return;
    }

    FILE* stream;
    if (level <= LogLevel::Error) {
        stream = stderr;
    } else {
        stream = stdout;
    }

    fwrite(str.buffer, sizeof(char), str.len, stream);
}

Logger global_logger = {
    default_log,
    NULL,
};
LogLevel global_max_log_level = LogLevel::Information;

template <LogLevel level>
static io::Result global_logger_write(C* c, void*, Str str) {
    global_logger.log(c, level, str);
    return io::Result::ok();
}

#define define_log_writer(name, level) \
    io::Writer name() { return {{global_logger_write<LogLevel::level>}, NULL}; }

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
