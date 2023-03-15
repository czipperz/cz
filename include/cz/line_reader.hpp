#pragma once

#include <cz/file.hpp>
#include <cz/string.hpp>
#include <cz/vector.hpp>

namespace cz {

struct Line_Reader {
    /// Persistent heap storage used in tick.
    cz::String read_buffer = {};

    /// Stores the first half of lines split between two reads.
    cz::String between_buffer = {};

    /// CRLF support.
    cz::Carriage_Return_Carry carry = {};

    ////////////////////////////////////////////////////////////////////////////
    // Lifecycle
    ////////////////////////////////////////////////////////////////////////////

    void init();
    void drop();

    ////////////////////////////////////////////////////////////////////////////
    // Main functions
    ////////////////////////////////////////////////////////////////////////////

    /// Reset the line reader state as if it was a newly inited object
    void reset();

    /// Read as much from the file as possible and put completed lines into
    /// results.  The last line's contents are put into between_buffer and
    /// should be flushed via `finish` when the file has closed.
    void read_input(cz::Input_File* file, cz::Allocator allocator, cz::Vector<cz::Str>* results);

    /// Flush the last line's contents.
    void finish(cz::Vector<cz::Str>* results);
};

}
