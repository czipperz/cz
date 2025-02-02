#include <czt/test_base.hpp>

#include <cz/process.hpp>

#ifndef _WIN32
using namespace cz;

static cz::Str escape(cz::Str arg, cz::String* temp) {
    temp->len = 0;
    Process::escape_arg(arg, temp, cz::heap_allocator());
    return *temp;
}

TEST_CASE("Process::escape_arg") {
    cz::String temp = {};
    CZ_DEFER(temp.drop(cz::heap_allocator()));
    CHECK(escape("hello", &temp) == "hello");
    CHECK(escape("one space", &temp) == "'one space'");
    CHECK(escape("two spaces here", &temp) == "'two spaces here'");
    CHECK(escape("$", &temp) == "'$'");
    CHECK(escape("oh&boy", &temp) == "'oh&boy'");
    CHECK(escape("oh&boy*$xo", &temp) == "'oh&boy*$xo'");
    CHECK(escape("''", &temp) == "\"''\"");
    CHECK(escape("''''", &temp) == "\"''''\"");
    CHECK(escape("hello!", &temp) == "'hello!'");
}
#endif
