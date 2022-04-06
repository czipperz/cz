#pragma once

#include "str.hpp"

namespace cz {

struct Str_Iter {
    cz::Str str;
    size_t index;

    bool at_bob() const { return index == 0; }
    bool at_eob() const { return index == str.len; }

    char get() const {
        CZ_DEBUG_ASSERT(index < str.len);
        return str[index];
    }

    void advance(size_t num = 1) {
        CZ_DEBUG_ASSERT(index + num <= str.len);
        index += num;
    }
    void retreat(size_t num = 1) {
        CZ_DEBUG_ASSERT(index >= num);
        index -= num;
    }
    void go_to(size_t pos) {
        CZ_DEBUG_ASSERT(pos < str.len);
        index = pos;
    }
};

void backward_char(Str_Iter* iter);
void forward_char(Str_Iter* iter);
void backward_word(Str_Iter* iter);
void forward_word(Str_Iter* iter);
void start_of_line(Str_Iter* iter);
void end_of_line(Str_Iter* iter);

bool find(Str_Iter* iter, char c);
bool find(Str_Iter* iter, cz::Str str);
bool rfind(Str_Iter* iter, char c);
bool rfind(Str_Iter* iter, cz::Str str);

bool looking_at(Str_Iter* iter, char c);
bool looking_at(Str_Iter* iter, cz::Str str);

}
