#pragma once

#include "context_decl.hpp"
#include "logger.hpp"
#include "allocator.hpp"
#include "temp_arena.hpp"

namespace cz {

struct Context {
    Allocator allocator;
    TempArena* temp;

    Logger logger;
    LogLevel max_log_level;
};

}
