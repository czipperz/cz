#include <czt/test_base.hpp>

#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <chrono>
#include <cz/defer.hpp>
#include <cz/heap.hpp>
#include <cz/sort.hpp>
#include <random>

using namespace cz;

template <class T>
static void insertion_sort(cz::Slice<T> slice) {
    generic_insertion_sort(slice.start(), slice.end(), generic_is_less_ptr<T>, generic_swap_ptr<T>);
}

template <class Data, class Rand>
static Data** make_datas(int iterations, size_t length, int order, Rand&& rand) {
    Data** datas = (Data**)calloc(sizeof(Data*), iterations);
    CZ_ASSERT(datas);
    for (int i = 0; i < iterations; ++i) {
        datas[i] = (Data*)malloc(sizeof(Data) * length);
        CZ_ASSERT(datas[i]);
        for (int j = 0; j < length; ++j) {
            datas[i][j] = j;
        }
    }

    if (order == 1) {
        for (int i = 0; i < iterations; ++i) {
            std::reverse(datas[i], datas[i] + length);
        }
    } else if (order == 2) {
        for (int i = 0; i < iterations; ++i) {
            std::shuffle(datas[i], datas[i] + length, rand);
        }
    }

    return datas;
}

template <class Data>
static void free_datas(Data** datas, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        free(datas[i]);
    }
    free(datas);
}

TEST_CASE("Sort benchmark") {
    std::random_device rd;
    std::mt19937 rand(rd());

    for (size_t length = 1; length <= 64; length *= 2) {
        for (int order = 0; order < 3; ++order) {
            int iterations = 16;

            uint64_t** datas1 = make_datas<uint64_t>(iterations, length, order, rand);
            CZ_DEFER(free_datas(datas1, iterations));
            uint64_t** datas2 = make_datas<uint64_t>(iterations, length, order, rand);
            CZ_DEFER(free_datas(datas2, iterations));

            for (int i = 0; i < iterations; ++i) {
                cz::sort(cz::Slice<uint64_t>{datas1[i], length});
                CHECK(is_sorted(cz::Slice<uint64_t>{datas1[i], length}));
            }
            for (int i = 0; i < iterations; ++i) {
                insertion_sort(cz::Slice<uint64_t>{datas2[i], length});
                CHECK(is_sorted(cz::Slice<uint64_t>{datas2[i], length}));
            }
        }
    }
}
