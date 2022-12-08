#if !defined(UNSIGNED) || !defined(SIGNED) || !defined(MIN) || !defined(MINS)
#error
#endif

#include <cz/stringify.hpp>

inline void append_num(cz::Allocator allocator, cz::String* string, UNSIGNED x) {
    size_t start = string->len;

    while (x >= 10) {
        append(allocator, string, (char)('0' + x % 10));
        x /= 10;
    }
    CZ_DEBUG_ASSERT(x < 10);
    append(allocator, string, (char)('0' + x));

    size_t end = (string->len - start) / 2;
    for (size_t i = 0; i < end; ++i) {
        cz::swap((*string)[start + i], (*string)[string->len - i - 1]);
    }
}

inline void append_num(cz::Allocator allocator, cz::String* string, SIGNED x) {
    if (x == MIN) {
        append(allocator, string, MINS);
        return;
    }

    if (x < 0) {
        append(allocator, string, '-');
        append_num(allocator, string, (UNSIGNED)-x);
    } else {
        append_num(allocator, string, (UNSIGNED)x);
    }
}

#undef UNSIGNED
#undef SIGNED
#undef MIN
#undef MINS
