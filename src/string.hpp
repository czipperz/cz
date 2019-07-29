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

    bool operator==(const Str& other) const;
    bool operator!=(const Str& other) const { return !(*this == other); }
};

class String {
    char* _buffer;
    size_t _len;
    size_t _cap;

public:
    String() : _buffer(NULL), _len(0), _cap(0) {}
    String(char* buffer, size_t len) : _buffer(buffer), _len(len), _cap(len) {}
    String(char* buffer, size_t len, size_t cap) : _buffer(buffer), _len(len), _cap(cap) {}

    void reserve(size_t extra);
    void append(Str str);

    void drop();

    char* buffer() { return _buffer; }
    const char* buffer() const { return _buffer; }
    size_t len() const { return _len; }
    size_t cap() const { return _cap; }

    Str as_str() const { return {_buffer, _len}; }

    operator Str() const { return as_str(); }

    bool operator==(const Str& other) const { return this->as_str() == other; }
    bool operator!=(const Str& other) const { return this->as_str() != other; }
};

}
