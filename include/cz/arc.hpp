#pragma once

#include <atomic>
#include <cz/assert.hpp>
#include <cz/heap.hpp>
#include <new>
#include <utility>

namespace cz {

template <class T>
struct Arc_Value {
    T value;
    std::atomic_uint32_t strong;
    std::atomic_uint32_t total;
};

template <class T>
struct Arc_Weak;

/// An atomically reference counted pointer.  It heap allocates the value along
/// with some information about how many references are active.  This struct is
/// thread safe however it **DOES NOT MAKE THE VALUE POINTED TO THREAD SAFE**!
///
/// A single `Arc` object can be `clone`d to create another `Arc` object which
/// both point to the same `T` object.  Note that after `drop` has been called the
/// `Arc` object itself is no longer usable.  Calling `drop` at the same time as
/// you use other usages of the same `Arc` object will cause a data race.  If you
/// instead `clone` the `Arc` you will not race with other other `Arc` objects.
template <class T>
struct Arc {
private:
    Arc_Value<T>* pointer;
    void init_general();

    friend struct Arc_Weak<T>;

public:
    /// Initialize the pointer by copying the `value`.
    void init_copy(const T& value);

    /// Initialize the pointer by constructing a value from the `args`.
    template <class... Args>
    void init_emplace(Args&&... args);

    /// Clone the pointer.  This increments the reference count.
    Arc<T> clone() const noexcept;

    /// Obtain a weak reference to the pointer.  See `Arc_Weak` for more details.
    Arc_Weak<T> clone_downgrade() const noexcept;

    /// Decrement the reference count and if the reference count is `0`,
    /// destroy the associated value by calling `value.drop()`.  Frees the
    /// memory allocated once all strong and weak pointers have been dropped.
    void drop() noexcept;

    /// Dereference the pointer.
    T& operator*() const noexcept;
    T* operator->() const noexcept;
    T* get() const noexcept;
};

/// A weak reference to the reference counted pointer.  A weak reference can be
/// upgraded to a strong reference if there still exists another strong reference.
///
/// Weak references have two main uses:
/// 1. Breaking dependency chains.  In a doubly linked list, one direction should store
///    `Arc` and the other should store `Arc_Weak`.  If this is not done then each node
///    will own the node before and after it and they will never automatically destruct.
///
/// 2. Allowing users to observe when a pointer has been destroyed.  A weak
///    pointer will fail to `upgrade` if the pointer has been destroyed.
template <class T>
struct Arc_Weak {
private:
    Arc_Value<T>* pointer;

    friend struct Arc<T>;

public:
    /// Try to upgrade this weak reference to a strong reference (`Arc`).
    /// If successful, the `Arc` must be destroyed via `drop`.
    bool upgrade(Arc<T>*) const noexcept;

    /// Test if the resource is still alive.  If this is `false` then `upgrade` will always fail.
    bool still_alive() const noexcept;

    /// Decrement the reference count.  Frees the memory allocated once all strong and weak
    /// pointers have been dropped.  Note that this method will never destroy the value because
    /// it doesn't have ownership over the value, only the storage for the reference count.
    void drop() noexcept;
};

template <class T>
void decrement_total(Arc_Value<T>*& pointer) {
    CZ_DEBUG_ASSERT(pointer);

    uint32_t total = pointer->total.fetch_sub(1);
    CZ_DEBUG_ASSERT(total > 0);

    if (total == 1) {
        cz::heap_allocator().dealloc(pointer);
        pointer = nullptr;
    }
}

template <class T>
void Arc<T>::init_general() {
    pointer = cz::heap_allocator().alloc<Arc_Value<T> >();
    CZ_ASSERT(pointer);
    std::atomic_init(&pointer->strong, (uint32_t)1);
    std::atomic_init(&pointer->total, (uint32_t)1);
}

template <class T>
void Arc<T>::init_copy(const T& value) {
    init_general();
    new (&pointer->value) T(value);
}

template <class T>
template <class... Args>
void Arc<T>::init_emplace(Args&&... args) {
    init_general();
    new (&pointer->value) T(std::forward<Args>(args)...);
}

template <class T>
Arc<T> Arc<T>::clone() const noexcept {
    CZ_DEBUG_ASSERT(pointer);

    uint32_t strong = pointer->strong.fetch_add(1);
    (void)strong;
    CZ_DEBUG_ASSERT(strong > 0);

    uint32_t total = pointer->total.fetch_add(1);
    (void)total;
    CZ_DEBUG_ASSERT(total > 0);

    Arc<T> copy;
    copy.pointer = pointer;
    return copy;
}

template <class T>
Arc_Weak<T> Arc<T>::clone_downgrade() const noexcept {
    CZ_DEBUG_ASSERT(pointer);

    uint32_t total = pointer->total.fetch_add(1);
    (void)total;
    CZ_DEBUG_ASSERT(total > 0);

    Arc_Weak<T> copy;
    copy.pointer = pointer;
    return copy;
}

template <class T>
void Arc<T>::drop() noexcept {
    CZ_DEBUG_ASSERT(pointer);

    uint32_t strong = pointer->strong.fetch_sub(1);
    CZ_DEBUG_ASSERT(strong > 0);

    if (strong == 1) {
        pointer->value.drop();
    }

    decrement_total(pointer);
}

template <class T>
T& Arc<T>::operator*() const noexcept {
    CZ_DEBUG_ASSERT(pointer);
    return pointer->value;
}

template <class T>
T* Arc<T>::operator->() const noexcept {
    return get();
}

template <class T>
T* Arc<T>::get() const noexcept {
    CZ_DEBUG_ASSERT(pointer);
    return &pointer->value;
}

template <class T>
bool Arc_Weak<T>::upgrade(Arc<T>* arc) const noexcept {
    CZ_DEBUG_ASSERT(pointer);

    uint32_t strong = pointer->strong.load();
    while (1) {
        // There are no strong references so the resource has been destroyed.
        if (strong == 0) {
            return false;
        }

        // Note: this updates count by taking it by reference.
        if (!pointer->strong.compare_exchange_weak(strong, strong + 1)) {
            continue;
        }

        // We secured a strong reference.
        pointer->total.fetch_add(1);
        arc->pointer = pointer;
        return true;
    }
}

template <class T>
bool Arc_Weak<T>::still_alive() const noexcept {
    CZ_DEBUG_ASSERT(pointer);

    return pointer->strong.load() > 0;
}

template <class T>
void Arc_Weak<T>::drop() noexcept {
    decrement_total(pointer);
}

}
