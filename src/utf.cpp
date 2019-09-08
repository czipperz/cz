#include <cz/utf.hpp>

#include <cz/assert.hpp>

namespace cz {
namespace utf8 {

static bool is_trailing_byte(uint8_t unit) {
    /* 10xxxxxx */
    return (unit & (1 << 7 | 1 << 6)) == (1 << 7);
}

static uint32_t trailing_byte_value(uint8_t unit) {
    /* __xxxxxx */
    return unit & (1 << 5 | 1 << 4 | 1 << 3 | 1 << 2 | 1 << 1 | 1);
}

static bool is_1_byte_sequence(uint8_t unit) {
    /* 0xxxxxxx */
    return (unit & (1 << 7)) == 0;
}

static bool is_2_byte_sequence(uint8_t unit) {
    /* 110xxxxx */
    return (unit & (1 << 7 | 1 << 6 | 1 << 5)) == (1 << 7 | 1 << 6);
}

static bool is_3_byte_sequence(uint8_t unit) {
    /* 1110xxxx */
    return (unit & (1 << 7 | 1 << 6 | 1 << 5 | 1 << 4)) == (1 << 7 | 1 << 6 | 1 << 5);
}

static bool is_4_byte_sequence(uint8_t unit) {
    /* 11110xxx */
    return (unit & (1 << 7 | 1 << 6 | 1 << 5 | 1 << 4 | 1 << 3)) ==
           (1 << 7 | 1 << 6 | 1 << 5 | 1 << 4);
}

bool is_valid(const uint8_t* buffer, size_t len) {
    const uint8_t* const end = buffer + len;

    // Deterministic finite automaton for utf8 parsing.  This is based on Bob
    // Steagall's talk at CppCon 2018:
    // https://www.youtube.com/watch?v=5FQ87-Ecb-A
start:
    if (buffer == end) {
        return true;
    } else if (is_1_byte_sequence(*buffer)) {
        ++buffer;
        goto start;
    } else if (*buffer >= 0xC2 && *buffer <= 0xDF) {
        ++buffer;
        goto cs1;
    } else if (*buffer == 0xE0) {
        ++buffer;
        goto p3a;
    } else if (*buffer == 0xED) {
        ++buffer;
        goto p3b;
    } else if (*buffer >= 0xE1 && *buffer <= 0xEF) {
        ++buffer;
        goto cs2;
    } else if (*buffer == 0xF0) {
        ++buffer;
        goto p4a;
    } else if (*buffer >= 0xF1 && *buffer <= 0xF3) {
        ++buffer;
        goto cs3;
    } else if (*buffer == 0xF4) {
        ++buffer;
        goto p4b;
    } else {
        return false;
    }

cs1:
    if (buffer != end && *buffer >= 0x80 && *buffer <= 0xBF) {
        ++buffer;
        goto start;
    } else {
        return false;
    }

p3a:
    if (buffer != end && *buffer >= 0xA0 && *buffer <= 0xBF) {
        ++buffer;
        goto cs1;
    } else {
        return false;
    }

p3b:
    if (buffer != end && *buffer >= 0x80 && *buffer <= 0x9F) {
        ++buffer;
        goto cs1;
    } else {
        return false;
    }

cs2:
    if (buffer != end && *buffer >= 0x80 && *buffer <= 0xBF) {
        ++buffer;
        goto cs1;
    } else {
        return false;
    }

p4a:
    if (buffer != end && *buffer >= 0x90 && *buffer <= 0xBF) {
        ++buffer;
        goto cs2;
    } else {
        return false;
    }

cs3:
    if (buffer != end && *buffer >= 0x80 && *buffer <= 0xBF) {
        ++buffer;
        goto cs2;
    } else {
        return false;
    }

p4b:
    if (buffer != end && *buffer >= 0x80 && *buffer <= 0x8F) {
        ++buffer;
        goto cs2;
    } else {
        return false;
    }
}

uint32_t to_utf32(const uint8_t* buffer) {
    if (is_1_byte_sequence(buffer[0])) {
        return buffer[0];
    } else if (is_2_byte_sequence(buffer[0])) {
        return ((buffer[0] & (1 << 4 | 1 << 3 | 1 << 2 | 1 << 1 | 1)) << 6) |
               trailing_byte_value(buffer[1]);
    } else if (is_3_byte_sequence(buffer[0])) {
        return ((buffer[0] & (1 << 3 | 1 << 2 | 1 << 1 | 1)) << 12) |
               (trailing_byte_value(buffer[1]) << 6) | trailing_byte_value(buffer[2]);
    } else if (is_4_byte_sequence(buffer[0])) {
        return ((buffer[0] & (1 << 2 | 1 << 1 | 1)) << 18) |
               (trailing_byte_value(buffer[1]) << 12) | (trailing_byte_value(buffer[2]) << 6) |
               trailing_byte_value(buffer[3]);
    } else {
        CZ_PANIC("cz::utf8::to_utf32(): Invalid utf8");
    }
}

size_t forward(const uint8_t* buffer) {
    if (is_1_byte_sequence(buffer[0])) {
        return 1;
    } else if (is_2_byte_sequence(buffer[0])) {
        return 2;
    } else if (is_3_byte_sequence(buffer[0])) {
        return 3;
    } else if (is_4_byte_sequence(buffer[0])) {
        return 4;
    } else {
        CZ_PANIC("cz::utf8::forward(): Invalid utf8");
    }
}

size_t backward(const uint8_t* buffer) {
    if (is_trailing_byte(buffer[-1])) {
        if (is_trailing_byte(buffer[-2])) {
            if (is_trailing_byte(buffer[-3])) {
                CZ_DEBUG_ASSERT(is_4_byte_sequence(buffer[-4]));
                return 4;
            }

            CZ_DEBUG_ASSERT(is_3_byte_sequence(buffer[-3]));
            return 3;
        }

        CZ_DEBUG_ASSERT(is_2_byte_sequence(buffer[-2]));
        return 2;
    }

    CZ_DEBUG_ASSERT(is_1_byte_sequence(buffer[-1]));
    return 1;
}

}
}
