#pragma once

#include "../assert.hpp"
#include "../mem.hpp"
#include "../util.hpp"
#include "mutex.hpp"

namespace cz {
namespace sync {

template <class T>
class Lock;

template <class T>
class Guard {
    const Lock<T>* _lock;

    void destroy() {
        if (_lock) {
            _lock->force_unlock();
        }
    }

public:
    Guard(const Lock<T>& lock) : _lock(&lock) { _lock->force_lock(); }
    ~Guard() { destroy(); }

    Guard(const Guard&) = delete;
    Guard& operator=(const Guard&) = delete;

    Guard(Guard&& other) : _lock(take(other._lock)) {}
    Guard& operator=(Guard&& other) {
        destroy();
        _lock = take(other._lock);
    }

    T& get() {
        CZ_DEBUG_ASSERT(_lock);
        return _lock->unsafe_get();
    }
};

template <class T>
class Lock {
    T _value;
    Mutex* _mutex;

public:
    Lock(T&& value)
        : _value(forward<T>(value)),
          _mutex(static_cast<Mutex*>(mem::alloc(sizeof(Mutex)))) {
        new (_mutex) Mutex;
    }
    ~Lock() { mem::dealloc(_mutex, sizeof(_mutex)); }

    Lock(const Lock&) = delete;
    Lock& operator=(const Lock&) = delete;

    Guard<T> lock() const { return Guard<T>(*this); }

    void force_lock() const { _mutex->lock(); }
    void force_unlock() const { _mutex->unlock(); }
    T& unsafe_get() const { return const_cast<T&>(_value); }
};

}
}
