#pragma once

#include <stdio.h>
#include "result.hpp"
#include "slice.hpp"
#include "string.hpp"
#include "try.hpp"

namespace cz {

struct Writer {
    Result (*write_str)(void* data, Str str);
    void* data;
};

Writer file_writer(FILE* file);
Writer string_writer(Allocated<String>* string);
inline Writer cout() {
    return file_writer(stdout);
}
inline Writer cerr() {
    return file_writer(stderr);
}

template <class T1, class T2, class... Ts>
Result write(Writer writer, T1 t1, T2 t2, Ts... ts) {
    CZ_TRY(write(writer, t1));
    return write(writer, t2, ts...);
}

template <class... Ts>
Result writeln(Writer writer, Ts... ts) {
    return write(writer, ts..., '\n');
}

template <class... Ts>
Result print(Ts... ts) {
    return write(cout(), ts...);
}

template <class... Ts>
Result println(Ts... ts) {
    return print(ts..., '\n');
}

inline Result write(Writer writer, Str str) {
    return writer.write_str(writer.data, str);
}
inline Result write(Writer writer, char ch) {
    return write(writer, Str{&ch, 1});
}

Result write(Writer writer, short);
Result write(Writer writer, unsigned short);
Result write(Writer writer, int);
Result write(Writer writer, unsigned int);
Result write(Writer writer, long);
Result write(Writer writer, unsigned long);
Result write(Writer writer, long long);
Result write(Writer writer, unsigned long long);

}
