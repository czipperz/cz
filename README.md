# cz

A common library for build data-oriented C++ applications.  This mostly contains
a variety of data structures, memory allocators, and utility classes.

Cz is licensed under the MIT license as described in the file `LICENSE` in the
root of the repository.

List of features:

* `Context` class allows injecting allocation and logging facilities to
  applicable functions.
* Explicit memory allocation.
* Explicit memory deallocation.
* Explicit memory copying.
* A formatting library.
* A logging library.
* `String` and `Vector<T>` data structures.
* `Str` and `Slice<T>` views into data structures.
* Helpful macro utilities.

## Building

Requirements: `cmake` and a C++ compiler that supports C++11.

Suggested: clone the repository and include it in your cmake build via
`add_subdirectory()`.

Alternative: Build locally using `cmake` and then link the resulting library
into your program.  The script `./build-release.sh` will do this automatically.

## Benchmarking

Clone Google Benchmark into `./benchmark` and then run `./build-release-bench.sh`.
