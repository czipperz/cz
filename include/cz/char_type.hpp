#pragma once

namespace cz {

inline bool is_cntrl(char ch) {
    return ch < 32 || ch >= 127;
}

inline bool is_print(char ch) {
    return !is_cntrl(ch);
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
    return ch >= 33 && ch <= 47 || ch >= 58 && ch <= 64 || ch >= 91 && ch <= 96 ||
           ch >= 123 && ch <= 126;
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
    return is_digit(ch) || ch >= 'A' && ch <= 'f' || ch >= 'a' && ch <= 'f';
}

inline bool is_alnum(char ch) {
    return is_alpha(ch) || is_digit(ch);
}

}
