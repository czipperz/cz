#include <cz/semaphore.hpp>

#include <stdlib.h>
#include <cz/assert.hpp>
#include <cz/heap.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#elif defined(__APPLE__)
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif

namespace cz {

// Convert a void* to the primitive type.
#ifdef _WIN32
static HANDLE h(void*& handle) {
    // Windows has pointer semantics for HANDLEs so try to store it inline.
    if (sizeof(HANDLE) <= sizeof(void*)) {
        return *(HANDLE*)&handle;
    } else {
        return *(HANDLE*)handle;
    }
}
#elif defined(__APPLE__)
static dispatch_semaphore_t* h(void* handle) {
    return (dispatch_semaphore_t*)handle;
}
#else
static sem_t* h(void* handle) {
    return (sem_t*)handle;
}
#endif

void Semaphore::init(uint32_t initial_value) {
#ifdef _WIN32
    if (sizeof(HANDLE) > sizeof(void*)) {
        handle = cz::heap_allocator().alloc<HANDLE>();
        CZ_ASSERT(handle);
    }

    HANDLE hh = CreateSemaphoreA(/*security_attributes=*/nullptr, initial_value,
                                 /*maximum=*/1 << 15, /*name=*/nullptr);

    if (sizeof(HANDLE) <= sizeof(void*)) {
        *(HANDLE*)&handle = hh;
    } else {
        *(HANDLE*)handle = hh;
    }
#elif defined(__APPLE__)
    handle = cz::heap_allocator().alloc<dispatch_semaphore_t>();
    CZ_ASSERT(handle);

    *h(handle) = dispatch_semaphore_create(initial_value);
#else
    handle = cz::heap_allocator().alloc<sem_t>();
    CZ_ASSERT(handle);

    int result = sem_init(h(handle), /*is_shared_between_processes=*/0, initial_value);
    CZ_ASSERT(result == 0);
#endif
}

void Semaphore::drop() {
#ifdef _WIN32
    CloseHandle(h(handle));
    if (sizeof(HANDLE) > sizeof(void*)) {
        cz::heap_allocator().dealloc((HANDLE*)handle);
    }
#elif defined(__APPLE__)
    dispatch_release(*h(handle));
#else
    sem_destroy(h(handle));
    cz::heap_allocator().dealloc(h(handle));
#endif
}

void Semaphore::release() {
#ifdef _WIN32
    DWORD ret = ReleaseSemaphore(h(handle), /*increment=*/1, /*previous_count_out=*/nullptr);
    CZ_ASSERT(ret != 0);
#elif defined(__APPLE__)
    dispatch_semaphore_signal(*h(handle));
#else
    int ret = sem_post(h(handle));
    CZ_ASSERT(ret == 0);
#endif
}

void Semaphore::acquire() {
#ifdef _WIN32
    DWORD ret = WaitForSingleObject(h(handle), /*milli_seconds=*/INFINITE);
    CZ_ASSERT(ret == WAIT_OBJECT_0);
#elif defined(__APPLE__)
    intptr_t ret = dispatch_semaphore_wait(*h(handle), DISPATCH_TIME_FOREVER);
    CZ_ASSERT(ret == 0);
#else
    int ret = sem_wait(h(handle));
    CZ_ASSERT(ret == 0);
#endif
}

bool Semaphore::try_acquire() {
#ifdef _WIN32
    // Note that this ignores errors.
    return WaitForSingleObject(h(handle), /*milli_seconds=*/0) == WAIT_OBJECT_0;
#elif defined(__APPLE__)
    // Note that this ignores errors.
    return dispatch_semaphore_wait(*h(handle), DISPATCH_TIME_NOW) == 0;
#else
    // Note that this ignores errors.
    return sem_trywait(h(handle)) == 0;
#endif
}

}
