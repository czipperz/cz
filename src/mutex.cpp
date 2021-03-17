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
static HANDLE h(void*& handle) {
    if (sizeof(HANDLE) <= sizeof(void*)) {
        return *(HANDLE*)&handle;
    } else {
        return *(HANDLE*)handle;
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
    if (sizeof(HANDLE) > sizeof(void*)) {
        handle = malloc(sizeof(HANDLE));
        CZ_ASSERT(handle);
    }

    HANDLE hh = CreateMutexA(/*security_attributes=*/nullptr, /*start_locked=*/false,
                             /*name=*/nullptr);

    if (sizeof(HANDLE) <= sizeof(void*)) {
        *(HANDLE*)&handle = hh;
    } else {
        *(HANDLE*)handle = hh;
    }
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
    CloseHandle(h(handle));
    if (sizeof(HANDLE) > sizeof(void*)) {
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
    DWORD ret = ReleaseMutex(h(handle));
    CZ_ASSERT(ret != 0);
#else
    int ret = pthread_mutex_unlock(h(handle));
    CZ_ASSERT(ret == 0);
#endif
}

void Mutex::lock() {
#ifdef _WIN32
    DWORD ret = WaitForSingleObject(h(handle), /*milli_seconds=*/INFINITE);
    CZ_ASSERT(ret == WAIT_OBJECT_0);
#else
    int ret = pthread_mutex_lock(h(handle));
    CZ_ASSERT(ret == 0);
#endif
}

bool Mutex::try_lock() {
#ifdef _WIN32
    // Note that this ignores errors.
    return WaitForSingleObject(h(handle), /*milli_seconds=*/0) == WAIT_OBJECT_0;
#else
    // Note that this ignores errors.
    return pthread_mutex_trylock(h(handle)) == 0;
#endif
}

}
