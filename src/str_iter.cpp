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
    const char* ptr = iter->after().find(needle);
    if (ptr) {
        iter->go_to_ptr(ptr);
        return true;
    } else {
        iter->go_to(iter->str.len);
        return false;
    }
}
bool find(Str_Iter* iter, cz::Str needle) {
    const char* ptr = iter->after().find(needle);
    if (ptr) {
        iter->go_to_ptr(ptr);
        return true;
    } else {
        iter->go_to(iter->str.len);
        return false;
    }
}

bool rfind(Str_Iter* iter, char needle) {
    const char* ptr = iter->after().rfind(needle);
    if (ptr) {
        iter->go_to_ptr(ptr);
        return true;
    } else {
        iter->go_to(0);
        return false;
    }
}
bool rfind(Str_Iter* iter, cz::Str needle) {
    const char* ptr = iter->before().rfind(needle);
    if (ptr) {
        iter->go_to_ptr(ptr);
        return true;
    } else {
        iter->go_to(0);
        return false;
    }
}

bool looking_at(Str_Iter* iter, char query) {
    return iter->after().starts_with(query);
}
bool looking_at(Str_Iter* iter, cz::Str query) {
    return iter->after().starts_with(query);
}

}
