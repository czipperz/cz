#include <cz/mutex.hpp>

#include <stdlib.h>
#include <cz/assert.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace cz {

// Convert a void* to the primitive type.
#ifdef _WIN32
static CRITICAL_SECTION* h(void*& handle) {
    if (sizeof(CRITICAL_SECTION) <= sizeof(void*)) {
        return (CRITICAL_SECTION*)&handle;
    } else {
        return (CRITICAL_SECTION*)handle;
    }
}
#else
static pthread_mutex_t* h(void*& handle) {
    if (sizeof(pthread_mutex_t) <= sizeof(void*)) {
        return *(pthread_mutex_t**)&handle;
    } else {
        return (pthread_mutex_t*)handle;
    }
}
#endif

void Mutex::init() {
#ifdef _WIN32
    if (sizeof(CRITICAL_SECTION) > sizeof(void*)) {
        handle = malloc(sizeof(CRITICAL_SECTION));
        CZ_ASSERT(handle);
    }

    InitializeCriticalSection(h(handle));
#else
    if (sizeof(pthread_mutex_t) > sizeof(void*)) {
        handle = malloc(sizeof(pthread_mutex_t));
        CZ_ASSERT(handle);
    }

    pthread_mutex_init(h(handle), /*attr=*/nullptr);
#endif
}

void Mutex::drop() {
#ifdef _WIN32
    DeleteCriticalSection(h(handle));
    if (sizeof(CRITICAL_SECTION) > sizeof(void*)) {
        free(handle);
    }
#else
    pthread_mutex_destroy(h(handle));
    if (sizeof(pthread_mutex_t) > sizeof(void*)) {
        free(handle);
    }
#endif
}

void Mutex::unlock() {
#ifdef _WIN32
    LeaveCriticalSection(h(handle));
#else
    int ret = pthread_mutex_unlock(h(handle));
    CZ_ASSERT(ret == 0);
#endif
}

void Mutex::lock() {
#ifdef _WIN32
    EnterCriticalSection(h(handle));
#else
    int ret = pthread_mutex_lock(h(handle));
    CZ_ASSERT(ret == 0);
#endif
}

bool Mutex::try_lock() {
#ifdef _WIN32
    // Note that this ignores errors.
    return TryEnterCriticalSection(h(handle));
#else
    // Note that this ignores errors.
    return pthread_mutex_trylock(h(handle)) == 0;
#endif
}

}
