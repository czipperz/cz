#pragma once

namespace cz {

struct Mutex;

/// The condition variable synchronization primitive.  Used in
/// combination with a `Mutex` to represent when a condition is hit.
struct Condition_Variable {
private:
    void* handle;

public:
    /// Create the condition variable.
    void init();

    /// Destroy the condition variable.  This is not thread safe.
    void drop();

    /// Unlock the mutex, wait until the condition is hit, and then relock the mutex.
    ///
    /// This can spuriously fail and therefore should be ran in a loop.
    ///
    /// The `Mutex` must be locked before this is called.
    void wait(Mutex* mutex);

    /// Wake one thread sleeping on this condition variable.
    ///
    /// The `Mutex` should be unlocked before this is called.
    void signal_one();

    /// Wake all threads sleeping on this condition variable.
    ///
    /// The `Mutex` should be unlocked before this is called.
    void signal_all();
};

}
