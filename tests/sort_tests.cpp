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

#ifdef NDEBUG

template <class T>
static void insertion_sort(cz::Slice<T> slice) {
    generic_insertion_sort(slice.start(), slice.end(), generic_is_less_ptr<T>, generic_swap_ptr<T>);
}

template <class Callback>
static long time(Callback&& callback) {
    auto start_time = std::chrono::high_resolution_clock::now();

    callback();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    return (long)elapsed.count();
}

template <class T>
static bool is_sorted(cz::Slice<T> slice) {
    for (size_t i = 0; i + 1 < slice.len; ++i) {
        if (slice[i] > slice[i + 1]) {
            return false;
        }
    }
    return true;
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

template <class Data, class Rand>
static void run_test(size_t length, int order, Rand&& rand) {
    int iterations = 1024;

    Data** datas1 = make_datas<Data>(iterations, length, order, rand);
    CZ_DEFER(free_datas(datas1, iterations));
    Data** datas2 = make_datas<Data>(iterations, length, order, rand);
    CZ_DEFER(free_datas(datas2, iterations));

    long sort_time = time([&]() {
        for (int i = 0; i < iterations; ++i) {
            cz::sort(cz::Slice<Data>{datas1[i], length});
        }
    });
    long insertion_time = time([&]() {
        for (int i = 0; i < iterations; ++i) {
            insertion_sort(cz::Slice<Data>{datas2[i], length});
        }
    });
    double ratio = (double)insertion_time / sort_time;
    printf("%9ld %9ld %9lf ", sort_time, insertion_time, ratio);
}

template <class Data, class Rand>
static void run_tests(size_t length, Rand&& rand) {
    printf("%-9ld ", (long)sizeof(Data));
    for (int order = 0; order < 3; ++order) {
        run_test<Data>(length, order, rand);
    }
    printf("\n");
}

template <class Data, size_t Size>
struct Wrapper {
    Data arr[Size];

    Wrapper& operator=(int num) {
        for (size_t i = 0; i < Size; ++i) {
            arr[i] = num;
        }
        return *this;
    }

    bool operator<(const Wrapper& other) const {
        size_t count = 0;
        for (size_t i = 0; i < Size; ++i) {
            if (arr[i] < other.arr[i]) {
                count++;
            }
        }
        return count == Size;
    }
};

TEST_CASE("Sort benchmark") {
    std::random_device rd;
    std::mt19937 rand(rd());

    for (size_t length = 1; length <= 64; length *= 2) {
        printf("length = %ld\n", (long)length);

        printf("%-9s %-29s %-29s %-29s\n", "", "sorted", "reverse", "random");
        printf("%-9s ", "bits");
        for (int order = 0; order < 3; ++order) {
            printf("%-9s %-9s %-9s ", "sort", "insertion", "ratio");
        }
        putchar('\n');

        run_tests<Wrapper<uint8_t, 1>>(length, rand);
        run_tests<Wrapper<uint16_t, 1>>(length, rand);
        run_tests<Wrapper<uint32_t, 1>>(length, rand);
        run_tests<Wrapper<uint64_t, 1>>(length, rand);
        run_tests<Wrapper<uint64_t, 2>>(length, rand);
        run_tests<Wrapper<uint64_t, 4>>(length, rand);
        run_tests<Wrapper<uint64_t, 8>>(length, rand);
        run_tests<Wrapper<uint64_t, 16>>(length, rand);
        run_tests<Wrapper<uint64_t, 32>>(length, rand);
        printf("\n");
    }
}

#endif
