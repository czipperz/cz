#pragma once

#include "allocator.hpp"
#include "arena.hpp"

namespace cz {

struct TempArena {
    Arena arena;
    size_t max_offset = 0;

    Allocator allocator();
};

}
