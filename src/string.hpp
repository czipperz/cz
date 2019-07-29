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
    constexpr String() : _buffer(NULL), _len(0), _cap(0) {}
    constexpr String(char* buffer, size_t len) : _buffer(buffer), _len(len), _cap(len) {}

    void reserve(size_t extra);
    void append(Str str);

    void drop();

    constexpr char* buffer() { return _buffer; }
    constexpr const char* buffer() const { return _buffer; }
    constexpr size_t len() const { return _len; }
    constexpr size_t cap() const { return _cap; }

    constexpr Str as_str() const { return {_buffer, _len}; }

    constexpr operator Str() const { return as_str(); }

    bool operator==(const Str& other) const { return this->as_str() == other; }
    bool operator!=(const Str& other) const { return this->as_str() != other; }
};

}
