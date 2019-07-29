#include <stddef.h>

namespace cz {

struct Str {
    const char* buffer;
    size_t len;

    constexpr Str() : buffer(NULL), len(0) {}
    Str(const char* cstr);
    constexpr Str(const char* buffer, size_t len) : buffer(buffer), len(len) {}

    template <size_t len>
    static constexpr Str cstr(const char (&str)[len]) {
        return {str, len - 1};
    }

    bool operator==(const char*) const;
    friend bool operator==(const char*, const Str&);
};

struct Buf {
    char* buffer;
    size_t len;
};

class String {
    char* _buffer;
    size_t _len;
    size_t _cap;

public:
    String();
    String(char* buffer, size_t len);

    void reserve(size_t extra);
    void append(Str str);

    void drop();

    char* buffer();
    const char* buffer() const;
    size_t len() const;
    size_t cap() const;

    Str as_str() const;
    Buf as_buf();

    bool operator==(const char*) const;
    friend bool operator==(const char*, const String&);

    bool operator==(const Str&) const;
    friend bool operator==(const Str&, const String&);
};

}
