#include <cz/mutex.hpp>

#include <cz/assert.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace cz {

#ifdef _WIN32
static_assert(sizeof(HANDLE) == sizeof(void*), "");
#else
static_assert(sizeof(pthread_mutex_t) == sizeof(void*), "");
#endif

void Mutex::init() {
#ifdef _WIN32
    HANDLE h = CreateMutexA(/*security_attributes=*/nullptr, /*start_locked=*/false,
                            /*name=*/nullptr);
    handle = *(void**)&h;
#else
    pthread_mutex_init((pthread_mutex_t*)&handle, /*attr=*/nullptr);
#endif
}

void Mutex::drop() {
#ifdef _WIN32
    CloseHandle(*(HANDLE*)&handle);
#else
    pthread_mutex_destroy((pthread_mutex_t*)&handle);
#endif
}

void Mutex::unlock() {
#ifdef _WIN32
    DWORD ret = ReleaseMutex(*(HANDLE*)&handle);
    CZ_ASSERT(ret != 0);
#else
    int ret = pthread_mutex_unlock((pthread_mutex_t*)&handle);
    CZ_ASSERT(ret == 0);
#endif
}

void Mutex::lock() {
#ifdef _WIN32
    DWORD ret = WaitForSingleObject(*(HANDLE*)&handle, /*milli_seconds=*/INFINITE);
    CZ_ASSERT(ret == WAIT_OBJECT_0);
#else
    int ret = pthread_mutex_lock((pthread_mutex_t*)&handle);
    CZ_ASSERT(ret == 0);
#endif
}

bool Mutex::try_lock() {
#ifdef _WIN32
    // Note that this ignores errors.
    return WaitForSingleObject(*(HANDLE*)&handle, /*milli_seconds=*/0) == WAIT_OBJECT_0;
#else
    // Note that this ignores errors.
    return pthread_mutex_trylock((pthread_mutex_t*)&handle) == 0;
#endif
}

}
