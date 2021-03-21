#include <cz/condition_variable.hpp>

#include <cz/mutex.hpp>

#include <stdlib.h>
#include <cz/assert.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <errno.h>
#include <pthread.h>
#endif

namespace cz {

// Convert a void* to the primitive type.
#ifdef _WIN32
static CONDITION_VARIABLE* h(void*& handle) {
    if (sizeof(CONDITION_VARIABLE) <= sizeof(void*)) {
        return (CONDITION_VARIABLE*)&handle;
    } else {
        return (CONDITION_VARIABLE*)handle;
    }
}
#else
static pthread_cond_t* h(void*& handle) {
    if (sizeof(pthread_cond_t) <= sizeof(void*)) {
        return *(pthread_cond_t**)&handle;
    } else {
        return (pthread_cond_t*)handle;
    }
}
#endif

void Condition_Variable::init() {
#ifdef _WIN32
    if (sizeof(CONDITION_VARIABLE) > sizeof(void*)) {
        handle = malloc(sizeof(CONDITION_VARIABLE));
        CZ_ASSERT(handle);
    }

    InitializeConditionVariable(h(handle));
#else
    if (sizeof(pthread_cond_t) > sizeof(void*)) {
        handle = malloc(sizeof(pthread_cond_t));
        CZ_ASSERT(handle);
    }

    pthread_cond_init(h(handle), /*attr=*/nullptr);
#endif
}

void Condition_Variable::drop() {
#ifdef _WIN32
    // No destroy function on Windows.
    if (sizeof(CONDITION_VARIABLE) > sizeof(void*)) {
        free(handle);
    }
#else
    pthread_cond_destroy(h(handle));
    if (sizeof(pthread_cond_t) > sizeof(void*)) {
        free(handle);
    }
#endif
}

// Convert a void* to the primitive type.
// @Condition_Variable_Mutex relies on this code.
#ifdef _WIN32
static CRITICAL_SECTION* hmutex(void*& handle) {
    if (sizeof(CRITICAL_SECTION) <= sizeof(void*)) {
        return (CRITICAL_SECTION*)&handle;
    } else {
        return (CRITICAL_SECTION*)handle;
    }
}
#else
static pthread_mutex_t* hmutex(void*& handle) {
    if (sizeof(pthread_mutex_t) <= sizeof(void*)) {
        return *(pthread_mutex_t**)&handle;
    } else {
        return (pthread_mutex_t*)handle;
    }
}
#endif

void Condition_Variable::wait(Mutex* mutex) {
#ifdef _WIN32
    // @Condition_Variable_Mutex relies on this code.
    SleepConditionVariableCS(h(handle), hmutex(mutex->handle), /*ms=*/INFINITE);
#else
    int ret = pthread_cond_wait(h(handle), hmutex(mutex->handle));
    if (ret == -1) {
        CZ_DEBUG_ASSERT(errno != EINVAL && "cz::Condition_Variable::wait(): Wrong mutex");
        CZ_DEBUG_ASSERT(errno != EPERM && "cz::Condition_Variable::wait(): Mutex wasn't locked");
    }
#endif
}

void Condition_Variable::signal_one() {
#ifdef _WIN32
    WakeConditionVariable(h(handle));
#else
    int ret = pthread_cond_signal(h(handle));
    if (ret == -1) {
        CZ_DEBUG_ASSERT(errno != EINVAL &&
                        "cz::Condition_Variable::signal_one(): Condition_Variable not initialized");
    }
#endif
}

void Condition_Variable::signal_all() {
#ifdef _WIN32
    WakeAllConditionVariable(h(handle));
#else
    int ret = pthread_cond_broadcast(h(handle));
    if (ret == -1) {
        CZ_DEBUG_ASSERT(errno != EINVAL &&
                        "cz::Condition_Variable::signal_one(): Condition_Variable not initialized");
    }
#endif
}

}
