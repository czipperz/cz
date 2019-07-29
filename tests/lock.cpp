#include "catch.hpp"

#include "../src/sync.hpp"

TEST_CASE("Mutex and LockGuard basically works") {
    cz::sync::Lock<int> lock(5);
    REQUIRE(lock.lock().get() == 5);
}

TEST_CASE("Locked move") {
    cz::sync::Lock<int> lock(5);
    auto locked = lock.lock();
}
