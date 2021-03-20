#pragma once

namespace cz {

struct Condition_Variable;

/// The mutex synchronization primitive.  Can be locked to control exclusive access to a resource.
///
/// A mutex is similar to a semaphore but has the following constraints:
/// 1. A mutex cannot be `unlock`ed by a different thread than the one that `lock`ed the mutex.
/// 2. A mutex only reprents the semaphore's values of `0` or `1`.
/// Because of these restrictions, a mutex is faster than a semaphore.
///
/// A semaphore can emulate a mutex by using `starting_value = 1`, and then using
/// `acquire` and `release` instead of `lock` and `unlock`.
struct Mutex {
private:
    void* handle;

    friend struct Condition_Variable;

public:
    /// Create the mutex.
    void init();

    /// Destroy the semaphore.  This is not thread safe.
    void drop();

    /// Unlock the mutex.
    void unlock();

    /// Lock the mutex.  Stalls until this is possible.
    void lock();

    /// Tries to lock the semaphore.  Returns `true` on success.
    ///
    /// This may fail for spurious reasons.
    bool try_lock();
};

}
