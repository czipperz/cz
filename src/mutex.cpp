#include <cz/mutex.hpp>

#include <stdlib.h>
#include <cz/assert.hpp>
#include <cz/heap.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace cz {

// Convert a void* to the primitive type.
// @Condition_Variable_Mutex relies on this code.
#ifdef _WIN32
static CRITICAL_SECTION* h(void* handle) {
    return (CRITICAL_SECTION*)handle;
}
#else
static pthread_mutex_t* h(void* handle) {
    return (pthread_mutex_t*)handle;
}
#endif

void Mutex::init() {
#ifdef _WIN32
    handle = cz::heap_allocator().alloc<CRITICAL_SECTION>();
    CZ_ASSERT(handle);

    InitializeCriticalSection(h(handle));
#else
    handle = cz::heap_allocator().alloc<pthread_mutex_t>();
    CZ_ASSERT(handle);

    pthread_mutex_init(h(handle), /*attr=*/nullptr);
#endif

#ifdef TRACY_ENABLE
    context = new tracy::SharedLockableCtx([]() -> const tracy::SourceLocationData* {
        static constexpr tracy::SourceLocationData srcloc{nullptr, "cz::Mutex", __FILE__, __LINE__,
                                                          0};
        return &srcloc;
    }());
#endif
}

void Mutex::drop() {
#ifdef _WIN32
    DeleteCriticalSection(h(handle));
#else
    pthread_mutex_destroy(h(handle));
#endif

    cz::heap_allocator().dealloc(h(handle));

#ifdef TRACY_ENABLE
    delete context;
#endif
}

void Mutex::unlock() {
#ifdef _WIN32
    LeaveCriticalSection(h(handle));
#else
    int ret = pthread_mutex_unlock(h(handle));
    CZ_ASSERT(ret == 0);
#endif

#ifdef TRACY_ENABLE
    context->AfterUnlock();
#endif
}

void Mutex::lock() {
#ifdef TRACY_ENABLE
    const auto run_after = context->BeforeLock();
#endif

#ifdef _WIN32
    EnterCriticalSection(h(handle));
#else
    int ret = pthread_mutex_lock(h(handle));
    CZ_ASSERT(ret == 0);
#endif

#ifdef TRACY_ENABLE
    if (run_after) {
        context->AfterLock();
    }
#endif
}

bool Mutex::try_lock() {
    bool success;

#ifdef _WIN32
    // Note that this ignores errors.
    success = TryEnterCriticalSection(h(handle));
#else
    // Note that this ignores errors.
    success = pthread_mutex_trylock(h(handle)) == 0;
#endif

#ifdef TRACY_ENABLE
    context->AfterTryLock(success);
#endif

    return success;
}

}
