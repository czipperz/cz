#pragma once

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
    /// Create a new empty \c String.
    String();
    /// Create a new \c String using the \c buffer with initial length and capacity \c len.
    String(char* buffer, size_t len);
    /// Create a new \c String using the \c buffer with initial length \c len and capacity \c cap.
    String(char* buffer, size_t len, size_t cap);

    /// Create a new \c String allocating a new buffer as a copy of the inputted string.
    explicit String(Str str_to_copy);

    /// Ensure there are \c extra bytes available in the buffer.
    void reserve(size_t extra);
    /// Append the \c Str to the buffer.
    void append(Str str);

    /// Set the \c len to \c 0.
    void clear();
    /// Set the \c len to \c new_len.  Panics if \c new_len is greater than \c len.
    void shrink_to(size_t new_len);
    /// Set the \c len to \c new_len.  Panics if \c new_len is greater than \c cap.
    void set_len(size_t new_len);

    /// Dealloc the \c buffer.
    void drop();

    /// Get the byte buffer backing the string.
    char* buffer();
    /// Get the byte buffer backing the string.
    const char* buffer() const;
    /// Get the length of the string in bytes.
    size_t len() const;
    /// Get the capacity of the string in bytes.
    size_t cap() const;

    /// Get a \c Str representing this \c String in its current state.
    Str as_str() const;
    /// See \c String::as_str().
    operator Str() const { return as_str(); }

    bool operator==(const Str& other) const { return this->as_str() == other; }
    bool operator!=(const Str& other) const { return this->as_str() != other; }
};

}
