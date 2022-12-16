#include <cz/parse.hpp>

#include <cz/char_type.hpp>
#include <limits>
#include <type_traits>

namespace cz {

///////////////////////////////////////////////////////////////////////////////

int64_t parse(cz::Str str, cz::Str pattern) {
    if (str.starts_with(pattern))
        return pattern.len;
    return 0;
}

int64_t parse(cz::Str str, char pattern) {
    if (str.starts_with(pattern))
        return 1;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

template <class U>
static int64_t parse_unsigned(cz::Str str, U* t) {
    U value = 0;

    for (size_t i = 0; i < str.len; ++i) {
        // Successful parse; return x.
        if (!cz::is_digit(str[i])) {
            *t = value;
            return i;
        }

        U v2 = value * 10;
        v2 += str[i] - '0';

        // Overflow is handled by eating the rest of the int and return -x.
        if (v2 < value) {
            ++i;
            for (; i < str.len; ++i) {
                if (!cz::is_digit(str[i])) {
                    break;
                }
            }
            return -(int64_t)i;
        }

        value = v2;
    }

    // Successful parse; return x.
    *t = value;
    return str.len;
}

template <class S>
static int64_t parse_signed(cz::Str str, S* s) {
    using U = typename std::make_unsigned<S>::type;

    if (str.len == 0) {
        return 0;
    }

    bool negative = str[0] == '-';
    if (negative) {
        str = str.slice_start(1);
    }

    U u;
    int64_t result = parse_unsigned(str, &u);
    if (result <= 0) {
        if (result < 0) {
            return result - negative;
        } else {
            return result;
        }
    }

    if (negative) {
        S min = std::numeric_limits<S>::min();
        if (u > (U)-min) {
            return -(result + negative);
        }
        *s = -(S)u;
    } else {
        S max = std::numeric_limits<S>::max();
        if (u > (U)max) {
            return -(result + negative);
        }
        *s = (S)u;
    }

    return result + negative;
}

///////////////////////////////////////////////////////////////////////////////

int64_t parse(cz::Str str, uint8_t* out) {
    return parse_unsigned(str, out);
}
int64_t parse(cz::Str str, uint16_t* out) {
    return parse_unsigned(str, out);
}
int64_t parse(cz::Str str, uint32_t* out) {
    return parse_unsigned(str, out);
}
int64_t parse(cz::Str str, uint64_t* out) {
    return parse_unsigned(str, out);
}

///////////////////////////////////////////////////////////////////////////////

int64_t parse(cz::Str str, int8_t* out) {
    return parse_signed(str, out);
}
int64_t parse(cz::Str str, int16_t* out) {
    return parse_signed(str, out);
}
int64_t parse(cz::Str str, int32_t* out) {
    return parse_signed(str, out);
}
int64_t parse(cz::Str str, int64_t* out) {
    return parse_signed(str, out);
}

///////////////////////////////////////////////////////////////////////////////

int64_t parse(cz::Str str, Parse_Rest rest) {
    *rest.out = str;
    return str.len;
}

int64_t parse(cz::Str str, Parse_Until_Char until) {
    size_t end = str.find_index(until.pattern);
    *until.out = str.slice_end(end);
    if (end < str.len)
        end += 1;
    return end;
}

int64_t parse(cz::Str str, Parse_Until_Str until) {
    size_t end = str.find_index(until.pattern);
    *until.out = str.slice_end(end);
    if (end < str.len)
        end += until.pattern.len;
    return end;
}

int64_t parse(cz::Str str, Parse_Before_Char until) {
    const char* end = str.find(until.pattern);
    if (end) {
        *until.out = str.slice_end(end);
        end += 1;
        return end - str.buffer;
    } else {
        return 0;
    }
}

int64_t parse(cz::Str str, Parse_Before_Str until) {
    const char* end = str.find(until.pattern);
    if (end) {
        *until.out = str.slice_end(end);
        end += until.pattern.len;
        return end - str.buffer;
    } else {
        return 0;
    }
}

int64_t parse(cz::Str str, Parse_Char ch) {
    if (str.len > 0) {
        *ch.out = str[0];
        return 1;
    } else {
        return 0;
    }
}

int64_t parse(cz::Str str, Parse_Word word) {
    size_t start = parse(str, skip_space());
    size_t end = start;
    for (; end < str.len; ++end) {
        if (cz::is_space(str[end]))
            break;
    }
    *word.out = str.slice(start, end);
    return end;
}

int64_t parse(cz::Str str, Parse_Skip_Space skip) {
    size_t i = 0;
    for (; i < str.len; ++i) {
        if (!cz::is_space(str[i]))
            break;
    }
    return i;
}

}
