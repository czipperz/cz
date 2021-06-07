#pragma once

namespace cz {

inline bool is_control(char ch) {
    return ch < 32 || ch >= 127;
}

inline bool is_print(char ch) {
    return !is_control(ch);
}

inline bool is_space(char ch) {
    switch (ch) {
        case ' ':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
        case '\v':
            return true;
        default:
            return false;
    }
}

inline bool is_blank(char ch) {
    return ch == '\t' || ch == ' ';
}

inline bool is_graph(char ch) {
    return ch >= 33 && ch < 127;
}

inline bool is_punct(char ch) {
    return (ch >= 33 && ch <= 47) || (ch >= 58 && ch <= 64) || (ch >= 91 && ch <= 96) ||
           (ch >= 123 && ch <= 126);
}

inline bool is_upper(char ch) {
    return ch >= 'A' && ch <= 'Z';
}

inline bool is_lower(char ch) {
    return ch >= 'a' && ch <= 'z';
}

inline char to_upper(char ch) {
    if (is_lower(ch)) {
        ch = ch - 'a' + 'A';
    }
    return ch;
}

inline char to_lower(char ch) {
    if (is_upper(ch)) {
        ch = ch - 'A' + 'a';
    }
    return ch;
}

inline bool is_alpha(char ch) {
    return is_upper(ch) || is_lower(ch);
}

inline bool is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}

inline bool is_hex_digit(char ch) {
    return is_digit(ch) || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f');
}

inline bool is_alnum(char ch) {
    return is_alpha(ch) || is_digit(ch);
}

#define CZ_DIGIT_CASES                                                                          \
    '0' : case '1' : case '2' : case '3' : case '4' : case '5' : case '6' : case '7' : case '8' \
        : case '9'

#define CZ_HEX_LETTER_CASES                                                                     \
    'a' : case 'b' : case 'c' : case 'd' : case 'e' : case 'f' : case 'A' : case 'B' : case 'C' \
        : case 'D' : case 'E' : case 'F'
#define CZ_HEX_DIGIT_CASES \
    CZ_DIGIT_CASES:        \
    case CZ_HEX_LETTER_CASES

#define CZ_LOWER_CASES                                                                          \
    'a' : case 'b' : case 'c' : case 'd' : case 'e' : case 'f' : case 'g' : case 'h' : case 'i' \
        : case 'j' : case 'k' : case 'l' : case 'm' : case 'n' : case 'o' : case 'p' : case 'q' \
        : case 'r' : case 's' : case 't' : case 'u' : case 'v' : case 'w' : case 'x' : case 'y' \
        : case 'z'

#define CZ_UPPER_CASES                                                                          \
    'A' : case 'B' : case 'C' : case 'D' : case 'E' : case 'F' : case 'G' : case 'H' : case 'I' \
        : case 'J' : case 'K' : case 'L' : case 'M' : case 'N' : case 'O' : case 'P' : case 'Q' \
        : case 'R' : case 'S' : case 'T' : case 'U' : case 'V' : case 'W' : case 'X' : case 'Y' \
        : case 'Z'

#define CZ_ALPHA_CASES \
    CZ_LOWER_CASES:    \
    case CZ_UPPER_CASES

}
