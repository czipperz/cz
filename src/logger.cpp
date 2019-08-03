#include "logger.hpp"

#include <iostream>
#include "mem.hpp"

namespace cz {
namespace log {

static thread_local LogLevel thread_local_level;
io::Result thread_local_level_write(void* _logger, Str str) {
    auto logger = static_cast<Logger*>(_logger);
    logger->log(thread_local_level, str);
    return io::Result::ok();
}

io::Writer Logger::writer_local(LogLevel level) {
    thread_local_level = level;
    return {
        {thread_local_level_write},
        this,
    };
}

struct LogWriter {
    Logger* logger;
    LogLevel level;
};

io::Result write_log_writer(void* _log_writer, Str str) {
    auto log_writer = static_cast<LogWriter*>(_log_writer);
    log_writer->logger->log(log_writer->level, str);
    return io::Result::ok();
}

io::Writer Logger::writer_alloc(LogLevel level) {
    auto log_writer = mem::global_allocator.alloc<LogWriter>();
    *log_writer = {this, level};
    return {
        {write_log_writer},
        log_writer,
    };
}

void Logger::writer_dealloc(io::Writer log_writer) {
    mem::global_allocator.dealloc({log_writer.data, sizeof(LogWriter)});
}

static void default_log(void*, LogLevel level, Str str) {
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

}
}
