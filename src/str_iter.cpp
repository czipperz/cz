#include <cz/str_iter.hpp>

namespace cz {

void backward_char(Str_Iter* iter) {
    if (!iter->at_bob())
        iter->retreat();
}

void forward_char(Str_Iter* iter) {
    if (!iter->at_eob())
        iter->advance();
}

void backward_word(Str_Iter* iter) {
    do {
        if (iter->at_bob())
            return;
        iter->retreat();
    } while (!cz::is_alnum(iter->get()));

    do {
        if (iter->at_bob())
            return;
        iter->retreat();
    } while (cz::is_alnum(iter->get()));

    iter->advance();
}

void forward_word(Str_Iter* iter) {
    while (!iter->at_eob() && !cz::is_alnum(iter->get())) {
        iter->advance();
    }
    while (!iter->at_eob() && cz::is_alnum(iter->get())) {
        iter->advance();
    }
}

void start_of_line(Str_Iter* iter) {
    (void)rfind(iter, '\n');
}

void end_of_line(Str_Iter* iter) {
    if (find(iter, '\n'))
        iter->advance();
}

bool find(Str_Iter* iter, char needle) {
    cz::Str after = iter->str.slice_start(iter->index);
    const char* ptr = after.find(needle);
    if (ptr) {
        iter->index = ptr - iter->str.buffer;
        return true;
    } else {
        iter->index = iter->str.len;
        return false;
    }
}
bool find(Str_Iter* iter, cz::Str needle) {
    cz::Str after = iter->str.slice_start(iter->index);
    const char* ptr = after.find(needle);
    if (ptr) {
        iter->index = ptr - iter->str.buffer;
        return true;
    } else {
        iter->index = iter->str.len;
        return false;
    }
}

bool rfind(Str_Iter* iter, char needle) {
    cz::Str before = iter->str.slice_end(iter->index);
    const char* ptr = before.rfind(needle);
    if (ptr) {
        iter->index = ptr - iter->str.buffer;
        return true;
    } else {
        iter->index = 0;
        return false;
    }
}
bool rfind(Str_Iter* iter, cz::Str needle) {
    cz::Str before = iter->str.slice_end(iter->index);
    const char* ptr = before.rfind(needle);
    if (ptr) {
        iter->index = ptr - iter->str.buffer;
        return true;
    } else {
        iter->index = 0;
        return false;
    }
}

bool looking_at(Str_Iter* iter, char query) {
    return iter->str.slice_start(iter->index).starts_with(query);
}
bool looking_at(Str_Iter* iter, cz::Str query) {
    return iter->str.slice_start(iter->index).starts_with(query);
}

}
