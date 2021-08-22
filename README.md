# cz

A common library for building data-oriented C++ applications.

Cz is licensed under GPL3.  If you wish to purchase a
different license, email czipperz AT gmail DOT com.

List of features:
* Explicit memory allocation and deallocation (`allocator.hpp`).
* Memory allocators (`arena.hpp`, `buffer_array.hpp`, `heap.hpp`).
* Basic data structures (`string.hpp`, `vector.hpp`, `str.hpp`, and `slice.hpp`).
* Threading library (`mutex.hpp`, `semaphore.hpp`, `condition_variable.hpp`).
* File system interface (`file.hpp`).
* Path manipulation (`path.hpp`).
* Process control (`process.hpp`).
* Environment and working directory manipulation (`env.hpp` and `working_directory.hpp`).
* Formatting objects to text (`format.hpp`).
* Parsing numbers from text (`parse.hpp`).
* Integration with Tracy.

## Building

Requirements: `cmake` and a C++ compiler that supports C++11.

Suggested: clone the repository and include it in your cmake build via `add_subdirectory()`.

Alternative: Build locally using `./build-release` and
then link the resulting library into your program.

## Benchmarking

Clone Google Benchmark into `./benchmark` and then run `./build-release-bench.sh`.
