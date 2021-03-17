#include <cz/semaphore.hpp>

#include <stdlib.h>
#include <cz/assert.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <semaphore.h>
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
static sem_t* h(void*& handle) {
    if (sizeof(sem_t) <= sizeof(void*)) {
        return *(sem_t**)&handle;
    } else {
        return (sem_t*)handle;
    }
}
#endif

void Semaphore::init(uint32_t initial_value) {
#ifdef _WIN32
    if (sizeof(HANDLE) > sizeof(void*)) {
        handle = malloc(sizeof(HANDLE));
        CZ_ASSERT(handle);
    }

    HANDLE hh = CreateSemaphoreA(/*security_attributes=*/nullptr, initial_value,
                                 /*maximum=*/1 << 15, /*name=*/nullptr);

    if (sizeof(HANDLE) <= sizeof(void*)) {
        *(HANDLE*)&handle = hh;
    } else {
        *(HANDLE*)handle = hh;
    }
#else
    if (sizeof(sem_t) > sizeof(void*)) {
        handle = malloc(sizeof(sem_t));
        CZ_ASSERT(handle);
    }

    sem_init(h(handle), /*is_shared_between_processes=*/0, initial_value);
#endif
}

void Semaphore::drop() {
#ifdef _WIN32
    CloseHandle(h(handle));
    if (sizeof(HANDLE) > sizeof(void*)) {
        free(handle);
    }
#else
    sem_destroy(h(handle));
    if (sizeof(sem_t) > sizeof(void*)) {
        free(handle);
    }
#endif
}

void Semaphore::release() {
#ifdef _WIN32
    DWORD ret = ReleaseSemaphore(h(handle), /*increment=*/1, /*previous_count_out=*/nullptr);
    CZ_ASSERT(ret != 0);
#else
    int ret = sem_post(h(handle));
    CZ_ASSERT(ret == 0);
#endif
}

void Semaphore::acquire() {
#ifdef _WIN32
    DWORD ret = WaitForSingleObject(h(handle), /*milli_seconds=*/INFINITE);
    CZ_ASSERT(ret == WAIT_OBJECT_0);
#else
    int ret = sem_wait(h(handle));
    CZ_ASSERT(ret == 0);
#endif
}

bool Semaphore::try_acquire() {
#ifdef _WIN32
    // Note that this ignores errors.
    return WaitForSingleObject(h(handle), /*milli_seconds=*/0) == WAIT_OBJECT_0;
#else
    // Note that this ignores errors.
    return sem_trywait(h(handle)) == 0;
#endif
}

}
