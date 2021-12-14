#include <cz/rwlock.hpp>

#include <cz/defer.hpp>

namespace cz {

enum {
    UNLOCKED = 0,
    LOCKED_WRITING = 1,
    LOCKED_READING_1 = 2,
};

void RWLock::init() {
    mutex.init();
    cond_var.init();
    state = UNLOCKED;
}

void RWLock::drop() {
    cond_var.drop();
    mutex.drop();
}

void RWLock::lock_reading() {
    mutex.lock();
    CZ_DEFER(mutex.unlock());

    while (state == LOCKED_WRITING) {
        cond_var.wait(&mutex);
    }

    if (state == UNLOCKED) {
        state = LOCKED_READING_1;
    } else {
        ++state;
    }
}
void RWLock::unlock_reading() {
    bool signal = false;
    {
        mutex.lock();
        CZ_DEFER(mutex.unlock());

        if (state == LOCKED_READING_1) {
            state = UNLOCKED;
            signal = true;
        } else {
            --state;
        }
    }

    if (signal)
        cond_var.signal_one();
}

void RWLock::lock_writing() {
    mutex.lock();
    CZ_DEFER(mutex.unlock());

    while (state != UNLOCKED) {
        cond_var.wait(&mutex);
    }

    state = LOCKED_WRITING;
}
void RWLock::unlock_writing() {
    {
        mutex.lock();
        CZ_DEFER(mutex.unlock());

        state = UNLOCKED;
    }

    cond_var.signal_one();
}

}
