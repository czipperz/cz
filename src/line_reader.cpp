#include <cz/line_reader.hpp>

#include <cz/heap.hpp>

namespace cz {

void Line_Reader::init() {
    read_buffer.reserve_exact(cz::heap_allocator(), 4096);
}
void Line_Reader::drop() {
    read_buffer.drop(cz::heap_allocator());
}

void Line_Reader::reset() {
    read_buffer.len = 0;
    between_buffer = {};
    carry = {};
}

void Line_Reader::read_input(cz::Input_File* file,
                             cz::Allocator allocator,
                             cz::Vector<cz::Str>* results) {
    while (1) {
        int64_t read_len = file->read_text(read_buffer.buffer, read_buffer.cap, &carry);
        if (read_len <= 0)
            break;

        read_buffer.len = read_len;

        ///////////////////////////////////////////////

        cz::Str str = read_buffer;
        size_t eol = str.find_index('\n');
        between_buffer.reserve_exact(allocator, eol);
        between_buffer.append(str.slice_end(eol));

        ///////////////////////////////////////////////

        while (eol != str.len) {
            results->reserve(cz::heap_allocator(), 1);
            results->push(between_buffer);
            between_buffer = {};

            str = str.slice_start(eol + 1);
            eol = str.find_index('\n');
            between_buffer.reserve_exact(allocator, eol);
            between_buffer.append(str.slice_end(eol));
        }
    }
}

void Line_Reader::finish(cz::Vector<cz::Str>* results) {
    if (between_buffer.len > 0) {
        results->reserve(cz::heap_allocator(), 1);
        results->push(between_buffer);
    }
    between_buffer = {};
}

}
