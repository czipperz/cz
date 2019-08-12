#pragma once

#include "context_decl.hpp"
#include "logger_decl.hpp"
#include "mem/allocator.hpp"
#include "mem/arena.hpp"

namespace cz {

struct Context {
    mem::Allocator allocator;
    mem::Allocator temp;

    log::Logger logger;
    log::LogLevel max_log_level;

    /// Log the following information.
    void log(log::LogInfo info) const {
        if (info.level <= max_log_level) {
            return logger.log(info);
        } else {
            return;
        }
    }
};

}
