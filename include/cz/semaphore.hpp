#pragma once

#include <stdint.h>

namespace cz {

/// The semaphore synchronization primitive.  A synchronized unsigned counter
/// that can be increment or decremented.  Decrement represents "acquiring"
/// access to a resource whereas increment represents "releasing" access.
///
/// A semaphore can emulate a mutex by using `starting_value = 1`, and then using
/// `acquire` and `release` instead of `lock` and `unlock`.  Unlike a mutex, a
/// semaphore can be `release`d by a different thread that `acquired` the semaphore.
///
/// A semaphore has pointer semantics.  So you are free to copy
/// or move the struct as long as you only `drop` it once.
struct Semaphore {
private:
    void* handle;

public:
    /// Create the semaphore and initialize the value.
    void init(uint32_t initial_value);

    /// Destroy the semaphore.  This is not thread safe.
    void drop();

    /// Increment the semaphore.
    void release();

    /// Stalls until the semaphore is positive then decrements it.
    void acquire();

    /// Tries to decrement the semaphore.  Returns `true` on success.
    ///
    /// This may fail for spurious reasons.
    bool try_acquire();
};

}
