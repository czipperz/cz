#include <benchmark/benchmark.h>

#include <inttypes.h>
#include <stdint.h>
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

template <size_t Size>
static void BM_insertion_sort(benchmark::State& state) {
    using Data = Wrapper<uint64_t, Size>;

    size_t length = state.range(0);

    std::random_device rd;
    std::mt19937 rand(rd());

    Data* data = (Data*)malloc(sizeof(Data) * length);
    CZ_DEFER(free(data));
    for (int j = 0; j < length; ++j) {
        data[j] = j;
    }

    for (auto _ : state) {
        state.PauseTiming();
        std::shuffle(data, data + length, rand);
        state.ResumeTiming();

        insertion_sort(cz::slice(data, length));
    }
}

BENCHMARK_TEMPLATE(BM_insertion_sort, 1)->DenseRange(2, 16, 2);
BENCHMARK_TEMPLATE(BM_insertion_sort, 4)->DenseRange(2, 16, 2);
BENCHMARK_TEMPLATE(BM_insertion_sort, 16)->DenseRange(2, 16, 2);
BENCHMARK_TEMPLATE(BM_insertion_sort, 64)->DenseRange(2, 16, 2);
BENCHMARK_TEMPLATE(BM_insertion_sort, 256)->DenseRange(2, 16, 2);

template <size_t Size>
static void BM_cz_sort(benchmark::State& state) {
    using Data = Wrapper<uint64_t, Size>;

    size_t length = state.range(0);

    std::random_device rd;
    std::mt19937 rand(rd());

    Data* data = (Data*)malloc(sizeof(Data) * length);
    CZ_DEFER(free(data));
    for (int j = 0; j < length; ++j) {
        data[j] = j;
    }

    for (auto _ : state) {
        state.PauseTiming();
        std::shuffle(data, data + length, rand);
        state.ResumeTiming();

        cz::sort(cz::slice(data, length));
    }
}

BENCHMARK_TEMPLATE(BM_cz_sort, 1)->RangeMultiplier(4)->Range(32, 1 << 16);
BENCHMARK_TEMPLATE(BM_cz_sort, 4)->RangeMultiplier(4)->Range(32, 1 << 16);
BENCHMARK_TEMPLATE(BM_cz_sort, 16)->RangeMultiplier(4)->Range(32, 1 << 16);
BENCHMARK_TEMPLATE(BM_cz_sort, 64)->RangeMultiplier(4)->Range(32, 1 << 16);
BENCHMARK_TEMPLATE(BM_cz_sort, 256)->RangeMultiplier(4)->Range(32, 1 << 16);

template <size_t Size>
static void BM_std_sort(benchmark::State& state) {
    using Data = Wrapper<uint64_t, Size>;

    size_t length = state.range(0);

    std::random_device rd;
    std::mt19937 rand(rd());

    Data* data = (Data*)malloc(sizeof(Data) * length);
    CZ_DEFER(free(data));
    for (int j = 0; j < length; ++j) {
        data[j] = j;
    }

    for (auto _ : state) {
        state.PauseTiming();
        std::shuffle(data, data + length, rand);
        state.ResumeTiming();

        std::sort(data, data + length);
    }
}

BENCHMARK_TEMPLATE(BM_std_sort, 1)->RangeMultiplier(4)->Range(32, 1 << 16);
BENCHMARK_TEMPLATE(BM_std_sort, 4)->RangeMultiplier(4)->Range(32, 1 << 16);
BENCHMARK_TEMPLATE(BM_std_sort, 16)->RangeMultiplier(4)->Range(32, 1 << 16);
BENCHMARK_TEMPLATE(BM_std_sort, 64)->RangeMultiplier(4)->Range(32, 1 << 16);
BENCHMARK_TEMPLATE(BM_std_sort, 256)->RangeMultiplier(4)->Range(32, 1 << 16);
