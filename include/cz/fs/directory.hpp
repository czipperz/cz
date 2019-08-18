#pragma once

#include "../io/result.hpp"
#include "../mem/allocator.hpp"
#include "../string.hpp"
#include "../vector.hpp"

namespace cz {
namespace fs {

class DirectoryIterator {
    mem::Allocator _allocator;
    String _file;
    bool _done = false;

#ifdef _WIN32
    /* HANDLE */ void* handle /* uninitialized */;
#else
    /* DIR* */ void* dir /* uninitialized */;
#endif

public:
    DirectoryIterator(mem::Allocator allocator) : _allocator(allocator) {}

    Str file() const { return _file; }
    bool done() const { return _done; }

    io::Result advance();
    io::Result destroy();

    friend io::Result iterate_files(const char* cstr_path, DirectoryIterator* iterator);
};

io::Result iterate_files(const char* cstr_path, DirectoryIterator* iterator);

io::Result files(mem::Allocator allocator, const char* cstr_path, Vector<String>* paths);

}
}
