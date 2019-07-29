namespace cz {

template <class T, class I>
struct RelPtr {
    I offset;

    constexpr T* get() const {
        auto self = reinterpret_cast<const char*>(this);
        auto elem = self + offset;
        return const_cast<T*>(reinterpret_cast<const T*>(elem));
    }

    constexpr T* operator->() const {
        return get();
    }

    constexpr T& operator*() const {
        return *get();
    }
};

}
