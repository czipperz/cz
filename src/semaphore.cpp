#include <cz/semaphore.hpp>

#include <cz/assert.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <semaphore.h>
#endif

namespace cz {

#ifdef _WIN32
static_assert(sizeof(HANDLE) == sizeof(void*), "");
#else
static_assert(sizeof(sem_t) == sizeof(void*), "");
#endif

void Semaphore::init(uint32_t initial_value) {
#ifdef _WIN32
    HANDLE h = CreateSemaphoreA(/*security_attributes=*/nullptr, initial_value,
                                /*maximum=*/1 << 15, /*name=*/nullptr);
    handle = *(void**)&h;
#else
    sem_init((sem_t*)&handle, /*is_shared_between_processes=*/0, initial_value);
#endif
}

void Semaphore::drop() {
#ifdef _WIN32
    CloseHandle(*(HANDLE*)&handle);
#else
    sem_destroy((sem_t*)&handle);
#endif
}

void Semaphore::release() {
#ifdef _WIN32
    DWORD ret =
        ReleaseSemaphore(*(HANDLE*)&handle, /*increment=*/1, /*previous_count_out=*/nullptr);
    CZ_ASSERT(ret != 0);
#else
    int ret = sem_post((sem_t*)&handle);
    CZ_ASSERT(ret == 0);
#endif
}

void Semaphore::acquire() {
#ifdef _WIN32
    DWORD ret = WaitForSingleObject(*(HANDLE*)&handle, /*milli_seconds=*/INFINITE);
    CZ_ASSERT(ret == WAIT_OBJECT_0);
#else
    int ret = sem_wait((sem_t*)&handle);
    CZ_ASSERT(ret == 0);
#endif
}

bool Semaphore::try_acquire() {
#ifdef _WIN32
    // Note that this ignores errors.
    return WaitForSingleObject(*(HANDLE*)&handle, /*milli_seconds=*/0) == WAIT_OBJECT_0;
#else
    // Note that this ignores errors.
    return sem_trywait((sem_t*)&handle) == 0;
#endif
}

}
