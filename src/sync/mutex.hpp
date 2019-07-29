#pragma once

#include <mutex>

namespace cz {
namespace sync {

class Mutex {
    std::mutex _inner;

public:
    Mutex() {}
    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;
    ~Mutex() {}

    void lock() { _inner.lock(); }
    void unlock() { _inner.unlock(); }
};

}
}
