#pragma once

#include <stddef.h>
#include "allocator.hpp"
#include "str.hpp"

namespace cz {

/// A mutable string.

/// ```
/// #include <cz/defer.hpp>
/// #include <cz/heap.hpp>
/// #include <cz/string.hpp>
///
/// cz::String<int> string = {};
/// CZ_DEFER(string.drop(cz::heap_allocator()));
///
/// string.reserve(cz::heap_allocator(), 3);
/// string.push('a');
/// string.push('3');
/// string.null_terminate();
///
/// CZ_ASSERT(string.find('3') == string.buffer + 1);
/// ```
struct String {
    char* buffer;
    size_t len;
    size_t cap;

    /// Dealloc the `String`.
    void drop(Allocator);

    ///
    /// Allocation methods.
    ///

    /// Ensure there are `extra` spaces available.  Amortizing expansion.
    void reserve(Allocator allocator, size_t extra) { reserve_total(allocator, extra + len); }
    void reserve_total(Allocator allocator, size_t total);
    /// Ensure there are `extra` spaces available.  Exact expansion.
    void reserve_exact(Allocator allocator, size_t extra) {
        reserve_exact_total(allocator, extra + len);
    }
    void reserve_exact_total(Allocator allocator, size_t total);

    /// Reallocate the buffer so that the length is the same as the capacity.
    /// If the reallocation fails, nothing happens.
    void realloc(Allocator);

    /// Reallocate the buffer so that the capacity is one greater than the length.
    /// Panics if the reallocation fails.
    void realloc_null_terminate(Allocator);

    /// Create a new `String` with the same contents in a unique memory buffer.
    String clone(Allocator allocator) const { return as_str().clone(allocator); }
    String clone_null_terminate(Allocator allocator) const {
        return as_str().clone_null_terminate(allocator);
    }

    ///
    /// Insertion methods.
    /// Must `reserve` space before attempting to insert.
    /// Indices must be manually bounds checked.
    ///

    /// Push character `ch` onto the end of the string.
    void push(char ch);
    /// Calls `push` `count` times.
    void push_many(char ch, size_t count);
    /// Append the string `str` to the buffer.
    void append(Str str);

    /// Push `'\0'` onto the end of the string without changing the length.
    void null_terminate();

    /// Insert the character `ch` into the middle of the buffer.
    /// Must manually bounds check!
    void insert(size_t index, char ch) { insert(index, {&ch, 1}); }
    /// Insert the string `str` into the middle of the buffer.
    /// Must manually bounds check!
    void insert(size_t index, Str str);

    ///
    /// Removal methods.
    /// Indices must be manually bounds checked.
    ///

    /// Pop the last character off the string.
    char pop();

    /// Removes the character at `index`.
    void remove(size_t index);

    /// Removes `count` characters starting at `index`.
    void remove_many(size_t index, size_t count);

    /// Removes all characters starting at `start` up to but not including `end`.
    void remove_range(size_t start, size_t end) {
        CZ_DEBUG_ASSERT(end >= start);
        return remove_many(start, end - start);
    }

    /// Replace all instances of before with after.
    void replace(char before, char after);

    ///
    /// Miscellaneous commands.
    ///

    constexpr size_t remaining() const { return cap - len; }

    /// Pointer iterators.
    char* start() { return buffer; }
    const char* start() const { return buffer; }
    char* begin() { return buffer; }
    const char* begin() const { return buffer; }
    char* end() { return buffer + len; }
    const char* end() const { return buffer + len; }

    /// Utility.
    char first() const {
        CZ_DEBUG_ASSERT(len > 0);
        return buffer[0];
    }
    char last() const {
        CZ_DEBUG_ASSERT(len > 0);
        return buffer[len - 1];
    }

    /// Logical string comparison.
    bool operator==(const Str& other) const { return this->as_str() == other; }
    bool operator!=(const Str& other) const { return this->as_str() != other; }
    bool operator<(const Str& other) const { return this->as_str() < other; }
    bool operator>(const Str& other) const { return this->as_str() > other; }
    bool operator<=(const Str& other) const { return this->as_str() <= other; }
    bool operator>=(const Str& other) const { return this->as_str() >= other; }

    /// Must manually bounds check!
    char operator[](size_t i) const { return get(i); }
    char& operator[](size_t i) { return get(i); }

    char get(size_t i) const {
        CZ_DEBUG_ASSERT(i < len);
        return buffer[i];
    }
    char& get(size_t i) {
        CZ_DEBUG_ASSERT(i < len);
        return buffer[i];
    }

    ///
    /// Str methods.
    ///

    bool equals_case_insensitive(Str prefix) const {
        return as_str().equals_case_insensitive(prefix);
    }

    bool starts_with(Str prefix) const { return as_str().starts_with(prefix); }
    bool ends_with(Str postfix) const { return as_str().ends_with(postfix); }

    bool starts_with_case_insensitive(Str prefix) const {
        return as_str().starts_with_case_insensitive(prefix);
    }
    bool ends_with_case_insensitive(Str postfix) const {
        return as_str().ends_with_case_insensitive(postfix);
    }

    bool starts_with(char c) const { return as_str().starts_with(c); }
    bool ends_with(char c) const { return as_str().ends_with(c); }
    bool starts_with_case_insensitive(char c) const {
        return as_str().starts_with_case_insensitive(c);
    }
    bool ends_with_case_insensitive(char c) const { return as_str().ends_with_case_insensitive(c); }

    size_t count(char c) const { return as_str().count(c); }

    bool contains(Str infix) const { return as_str().contains(infix); }
    bool contains(char infix) const { return as_str().contains(infix); }
    bool contains_case_insensitive(Str infix) const {
        return as_str().contains_case_insensitive(infix);
    }
    bool contains_case_insensitive(char infix) const {
        return as_str().contains_case_insensitive(infix);
    }

    char* find(Str pattern) { return const_cast<char*>(as_str().find(pattern)); }
    const char* find(Str pattern) const { return as_str().find(pattern); }

    char* find(char pattern) { return const_cast<char*>(as_str().find(pattern)); }
    const char* find(char pattern) const { return as_str().find(pattern); }

    char* rfind(Str pattern) { return const_cast<char*>(as_str().rfind(pattern)); }
    const char* rfind(Str pattern) const { return as_str().rfind(pattern); }

    char* rfind(char pattern) { return const_cast<char*>(as_str().rfind(pattern)); }
    const char* rfind(char pattern) const { return as_str().rfind(pattern); }

    char* find_case_insensitive(Str pattern) {
        return const_cast<char*>(as_str().find_case_insensitive(pattern));
    }
    const char* find_case_insensitive(Str pattern) const {
        return as_str().find_case_insensitive(pattern);
    }

    char* find_case_insensitive(char pattern) {
        return const_cast<char*>(as_str().find_case_insensitive(pattern));
    }
    const char* find_case_insensitive(char pattern) const {
        return as_str().find_case_insensitive(pattern);
    }

    char* rfind_case_insensitive(Str pattern) {
        return const_cast<char*>(as_str().rfind_case_insensitive(pattern));
    }
    const char* rfind_case_insensitive(Str pattern) const {
        return as_str().rfind_case_insensitive(pattern);
    }

    char* rfind_case_insensitive(char pattern) {
        return const_cast<char*>(as_str().rfind_case_insensitive(pattern));
    }
    const char* rfind_case_insensitive(char pattern) const {
        return as_str().rfind_case_insensitive(pattern);
    }

    /// Or overloads return `otherwise` on no match.
    size_t find_or(Str infix, size_t otherwise) const { return as_str().find_or(infix, otherwise); }
    size_t rfind_or(Str infix, size_t otherwise) const {
        return as_str().rfind_or(infix, otherwise);
    }
    size_t find_or(char pattern, size_t otherwise) const {
        return as_str().find_or(pattern, otherwise);
    }
    size_t rfind_or(char pattern, size_t otherwise) const {
        return as_str().rfind_or(pattern, otherwise);
    }
    size_t find_or_case_insensitive(Str infix, size_t otherwise) const {
        return as_str().find_or_case_insensitive(infix, otherwise);
    }
    size_t rfind_or_case_insensitive(Str infix, size_t otherwise) const {
        return as_str().rfind_or_case_insensitive(infix, otherwise);
    }
    size_t find_or_case_insensitive(char pattern, size_t otherwise) const {
        return as_str().find_or_case_insensitive(pattern, otherwise);
    }
    size_t rfind_or_case_insensitive(char pattern, size_t otherwise) const {
        return as_str().rfind_or_case_insensitive(pattern, otherwise);
    }

    /// Index overloads return `len` on no match.
    size_t find_index(Str infix) const { return as_str().find_index(infix); }
    size_t rfind_index(Str infix) const { return as_str().rfind_index(infix); }
    size_t find_index(char pattern) const { return as_str().find_index(pattern); }
    size_t rfind_index(char pattern) const { return as_str().rfind_index(pattern); }
    size_t find_index_case_insensitive(Str infix) const {
        return as_str().find_index_case_insensitive(infix);
    }
    size_t rfind_index_case_insensitive(Str infix) const {
        return as_str().rfind_index_case_insensitive(infix);
    }
    size_t find_index_case_insensitive(char pattern) const {
        return as_str().find_index_case_insensitive(pattern);
    }
    size_t rfind_index_case_insensitive(char pattern) const {
        return as_str().rfind_index_case_insensitive(pattern);
    }

    bool split_excluding(char separator, cz::Str* before, cz::Str* after) const {
        return as_str().split_excluding(separator, before, after);
    }
    bool split_excluding(cz::Str separator, cz::Str* before, cz::Str* after) const {
        return as_str().split_excluding(separator, before, after);
    }
    bool split_before(char separator, cz::Str* before, cz::Str* after) const {
        return as_str().split_before(separator, before, after);
    }
    bool split_before(cz::Str separator, cz::Str* before, cz::Str* after) const {
        return as_str().split_before(separator, before, after);
    }
    bool split_after(char separator, cz::Str* before, cz::Str* after) const {
        return as_str().split_after(separator, before, after);
    }
    bool split_after(cz::Str separator, cz::Str* before, cz::Str* after) const {
        return as_str().split_after(separator, before, after);
    }

    bool split_excluding_last(char separator, cz::Str* before, cz::Str* after) const {
        return as_str().split_excluding_last(separator, before, after);
    }
    bool split_excluding_last(cz::Str separator, cz::Str* before, cz::Str* after) const {
        return as_str().split_excluding_last(separator, before, after);
    }
    bool split_before_last(char separator, cz::Str* before, cz::Str* after) const {
        return as_str().split_before_last(separator, before, after);
    }
    bool split_before_last(cz::Str separator, cz::Str* before, cz::Str* after) const {
        return as_str().split_before_last(separator, before, after);
    }
    bool split_after_last(char separator, cz::Str* before, cz::Str* after) const {
        return as_str().split_after_last(separator, before, after);
    }
    bool split_after_last(cz::Str separator, cz::Str* before, cz::Str* after) const {
        return as_str().split_after_last(separator, before, after);
    }

    void split_into(char separator, cz::Allocator allocator, cz::Vector<cz::Str>* values) {
        as_str().split_into(separator, allocator, values);
    }
    void split_into(cz::Str separator, cz::Allocator allocator, cz::Vector<cz::Str>* values) {
        as_str().split_into(separator, allocator, values);
    }
    void split_clone(char separator,
                     cz::Allocator vector_allocator,
                     cz::Allocator string_allocator,
                     cz::Vector<cz::Str>* values) {
        as_str().split_clone(separator, vector_allocator, string_allocator, values);
    }
    void split_clone(cz::Str separator,
                     cz::Allocator vector_allocator,
                     cz::Allocator string_allocator,
                     cz::Vector<cz::Str>* values) {
        as_str().split_clone(separator, vector_allocator, string_allocator, values);
    }
    void split_clone_nt(char separator,
                        cz::Allocator vector_allocator,
                        cz::Allocator string_allocator,
                        cz::Vector<cz::Str>* values) {
        as_str().split_clone_nt(separator, vector_allocator, string_allocator, values);
    }
    void split_clone_nt(cz::Str separator,
                        cz::Allocator vector_allocator,
                        cz::Allocator string_allocator,
                        cz::Vector<cz::Str>* values) {
        as_str().split_clone_nt(separator, vector_allocator, string_allocator, values);
    }
    void lines(cz::Allocator allocator, cz::Vector<cz::Str>* values) {
        as_str().lines(allocator, values);
    }

    /// Take a substring of the string.
    cz::Str slice(size_t start, size_t end) const { return as_str().slice(start, end); }
    cz::Str slice(const char* start, size_t end) const { return as_str().slice(start, end); }
    cz::Str slice(size_t start, const char* end) const { return as_str().slice(start, end); }
    cz::Str slice(const char* start, const char* end) const { return as_str().slice(start, end); }

    cz::Str slice_start(size_t start) const { return as_str().slice_start(start); }
    cz::Str slice_start(const char* start) const { return as_str().slice_start(start); }

    cz::Str slice_end(size_t end) const { return as_str().slice_end(end); }
    cz::Str slice_end(const char* end) const { return as_str().slice_end(end); }

    /// Get a `Str` representing this `String` in its current state.
    constexpr Str as_str() const { return {buffer, len}; }
    /// See `String::as_str()`.
    operator Str() const { return as_str(); }
};

}
