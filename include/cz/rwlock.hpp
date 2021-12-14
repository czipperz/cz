#pragma once

#include <stdint.h>
#include "condition_variable.hpp"
#include "mutex.hpp"

namespace cz {

struct RWLock {
    Mutex mutex;
    Condition_Variable cond_var;
    uint64_t state;

    void init();
    void drop();

    void lock_reading();
    void unlock_reading();
    void lock_writing();
    void unlock_writing();
};

}
