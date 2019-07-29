#include "catch.hpp"

#include "../src/relptr.hpp"

struct S {
    char bytes[8];
};

TEST_CASE("RelPtr get") {
    cz::RelPtr<S, int> ptr = {15};
    REQUIRE(ptr.get() == (S*)((char*)&ptr + 15));
}

TEST_CASE("RelPtr ->") {
    cz::RelPtr<S, int> ptr = {12};
    REQUIRE(&ptr->bytes[3] == (char*)&ptr + 15);
}

TEST_CASE("RelPtr *") {
    cz::RelPtr<S, int> ptr = {12};
    REQUIRE(&(*ptr).bytes[3] == (char*)&ptr + 15);
}
