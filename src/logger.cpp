#include "assert.hpp"

namespace cz {
namespace log {

struct LogWriter {
    Logger* logger;
    LogLevel level;
};

static void log_ignore(C*, MemSlice, LogLevel, Str) {
    return;
}

Logger ignore() {
    return {log_ignore, {}};
}

static void log_console(C*, MemSlice, LogLevel level, Str str) {
    FILE* stream;
    if (level <= LogLevel::Error) {
        stream = stderr;
    } else {
        stream = stdout;
    }

    fwrite(str.buffer, sizeof(char), str.len, stream);
}

Logger console() {
    return {log_console, {}};
}

static void log_string(C* c, MemSlice _string, LogLevel, Str str) {
    auto string = static_cast<String*>(_string.buffer);
    write(c, io::string_writer(string), str, '\n');
}

Logger string(C* c) {
    String* string = c->alloc<String>();
    CZ_ASSERT(c, string != NULL);
    return {log_string, slice(string, 1)};
}

void Logger::drop(C* c) {
    c->dealloc(data);
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
