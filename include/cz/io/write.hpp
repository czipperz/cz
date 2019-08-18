#pragma once

#include <stdio.h>
#include "../slice.hpp"
#include "../string.hpp"
#include "../try.hpp"
#include "result.hpp"

namespace cz {
namespace io {

struct Write {
    Result (*write_str)(void* data, Str str);
};

struct Writer {
    Write write;
    void* data;

    Result write_str(Str str) { return write.write_str(data, str); }
};

Writer file_writer(FILE* file);
Writer string_writer(mem::Allocated<String>* string);
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
Result put(Ts... ts) {
    return write(cout(), ts..., '\n');
}

inline Result write(Writer writer, char ch) {
    return writer.write_str({&ch, 1});
}
inline Result write(Writer writer, Str str) {
    return writer.write_str(str);
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
}