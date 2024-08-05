#include <cz/encode.hpp>

namespace cz {

static inline char encode_hex_digit(uint8_t v) {
    if (v < 10)
        return '0' + v;
    else
        return 'a' + v - 10;
}
static inline uint8_t decode_hex_digit(char v) {
    if (v >= '0' && v <= '9')
        return v - '0';
    else
        return 10 + v - 'a';
}

void encode_hex(cz::Str input, cz::Allocator allocator, cz::String* output) {
    output->reserve(allocator, input.len * 2);
    for (size_t i = 0; i < input.len; ++i) {
        uint8_t val = (uint8_t)input[i];
        output->push(encode_hex_digit(val >> 4));
        output->push(encode_hex_digit(val & 0xf));
    }
}

void decode_hex(cz::Str input, cz::Allocator allocator, cz::String* output) {
    output->reserve(allocator, (input.len + 1) / 2);
    for (size_t i = 0; i < input.len; i += 2) {
        char first = input[i];
        char second = '0';
        if (i + 1 < input.len)
            second = input[i + 1];

        uint8_t val = decode_hex_digit(first) << 4 | decode_hex_digit(second);
        output->push((char)val);
    }
}

static inline uint8_t encode_base64_digit(char digit) {
    if (digit < 26)
        return 'A' + digit;
    else if (digit < 52)
        return 'a' + digit - 26;
    else if (digit < 62)
        return '0' + digit - 52;
    else if (digit == 62)
        return '+';
    else
        return '/';
}

static inline uint32_t decode_base64_digit(char digit) {
    if (digit >= 'A' && digit <= 'Z')
        return digit - 'A';
    else if (digit >= 'a' && digit <= 'z')
        return digit - 'a' + 26;
    else if (digit >= '0' && digit <= '9')
        return digit - '0' + 52;
    else if (digit == '+')
        return 62;
    else
        return 63;
}

void encode_base64(cz::Str input, cz::Allocator allocator, cz::String* output) {
    output->reserve(allocator, (input.len + 2) / 3 * 4);

    for (size_t i = 0; i < input.len; i += 3) {
        uint32_t num = 0;
        size_t digits = 1;
        for (size_t j = 0; j < 3; ++j, ++digits) {
            if (i + j >= input.len)
                break;
            size_t shift = 16 - 8 * j;
            num |= ((uint8_t)input[i + j] << shift);
        }

        for (size_t j = 0; j < digits; ++j) {
            size_t shift = 18 - 6 * j;
            uint8_t digit = (num >> shift) & 0x3f;
            output->push(encode_base64_digit(digit));
        }
    }

    // Add padding.
    size_t extra = 3 - input.len % 3;
    if (extra < 3) {
        while (extra-- > 0)
            output->push('=');
    }
}

void decode_base64(cz::Str input, cz::Allocator allocator, cz::String* output) {
    output->reserve(allocator, (input.len + 3) / 4 * 3);

    for (size_t i = 0; i < input.len; i += 4) {
        uint32_t num = 0;
        size_t digits = -1;
        for (size_t j = 0; j < 4; ++j, ++digits) {
            if (i + j >= input.len)
                break;
            char ch = input[i + j];
            if (ch == '=')
                break;

            size_t shift = 6 * (3 - j);
            num |= (decode_base64_digit(ch) << shift);
        }

        for (size_t j = 0; j < digits; ++j) {
            size_t shift = 8 * (2 - j);
            uint8_t digit = (num >> shift) & 0xff;
            output->push(digit);
        }
    }
}

}
