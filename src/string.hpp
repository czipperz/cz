#include <stddef.h>

namespace cz {

struct Str {
    const char* start;
    size_t len;

    static Str from(const char* str);

    template <size_t len>
    static constexpr Str cstr(const char (&str)[len]) {
        return {str, len - 1};
    }
};

class String {
    char* start;
    size_t len;
    size_t cap;

public:
    String();
    String(char* buffer);
    String(char* buffer, size_t len);

    void drop();

    Str as_str();
};

}
