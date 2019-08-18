#include "test_base.hpp"

#include "context.hpp"
#include "cz/array_ref.hpp"
#include "cz/list.hpp"
#include "mem/mock_allocate.hpp"

using namespace cz;
using namespace cz::test;

TEST_CASE("List starting empty can be appended to") {
    int array[3] = {0};
    ArrayRef<int> list(array, 0, 3);
    list.push(4);

    CHECK(array[0] == 4);
    CHECK(array[1] == 0);
    CHECK(array[2] == 0);
}

TEST_CASE("List starting non-empty can be appended to") {
    int array[5] = {3, 4, 5, 6};
    ArrayRef<int> list(array, 3, 4);
    list.push(30);

    CHECK(array[0] == 3);
    CHECK(array[1] == 4);
    CHECK(array[2] == 5);
    CHECK(array[3] == 30);
    CHECK(array[4] == 0);
}

TEST_CASE("List insertion at beginning") {
    int array[5] = {3, 4, 5, 6};
    ArrayRef<int> list(array, 3, 4);
    list.insert(0, 30);

    CHECK(array[0] == 30);
    CHECK(array[1] == 3);
    CHECK(array[2] == 4);
    CHECK(array[3] == 5);
    CHECK(array[4] == 0);
}

TEST_CASE("List insertion in middle") {
    int array[5] = {3, 4, 5, 6};
    ArrayRef<int> list(array, 3, 4);
    list.insert(1, 30);

    CHECK(array[0] == 3);
    CHECK(array[1] == 30);
    CHECK(array[2] == 4);
    CHECK(array[3] == 5);
    CHECK(array[4] == 0);
}

TEST_CASE("List insertion at end") {
    int array[5] = {3, 4, 5, 6};
    ArrayRef<int> list(array, 3, 4);
    list.insert(3, 30);

    CHECK(array[0] == 3);
    CHECK(array[1] == 4);
    CHECK(array[2] == 5);
    CHECK(array[3] == 30);
    CHECK(array[4] == 0);
}
