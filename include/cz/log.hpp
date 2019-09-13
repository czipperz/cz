#pragma once

#include "context.hpp"
#include "defer.hpp"
#include "format.hpp"
#include "logger.hpp"

namespace cz {

struct LogWriter {
    Logger logger;
    LogInfo info;

    Writer writer();
};

template <class... Ts>
Result log(C* c, LogLevel level, const char* file, size_t line, Ts... ts) {
    if (level <= c->max_log_level) {
        LogWriter log_writer = {c->logger, {file, line, level}};
        CZ_TRY(c->logger.write_prefix(log_writer.info));
        CZ_TRY(cz::write(log_writer.writer(), ts...));
        CZ_TRY(c->logger.write_suffix(log_writer.info));
    }
    return Result::ok();
}

#define CZ_LOG(c, level, ...) (CZ_LOGL(c, cz::LogLevel::level, __VA_ARGS__))
#define CZ_LOGL(c, level, ...) (cz::log(c, level, __FILE__, __LINE__, __VA_ARGS__))

}
