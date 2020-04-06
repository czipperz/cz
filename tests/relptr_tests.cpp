#include <czt/test_base.hpp>

#include <cz/relptr.hpp>

struct S {
    char bytes[8];
};

TEST_CASE("RelPtr set") {
    cz::RelPtr<S, int> ptr;
    S s;
    ptr.set(&s);
    REQUIRE(ptr.get() == &s);
}

TEST_CASE("RelPtr copy is bitwise") {
    struct {
        cz::RelPtr<S, int> ptr;
        S s;
    } array[2];
    array[0].ptr.set(&array[0].s);
    array[1].ptr = array[0].ptr;
    REQUIRE(array[1].ptr.get() == &array[1].s);
}

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
