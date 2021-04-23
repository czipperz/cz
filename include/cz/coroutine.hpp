#pragma once

/// A `Coroutine` is a user-space thread that yields priority at
/// will.  This is useful for when you need lightweight threads.
///
/// Note that under the hood `CZ_CO_START` is a `swtich` statement and `CZ_CO_YIELD` is a `return`
/// statement.  Using a local variable or `CZ_DEFER` in conjugation with a `Coroutine` is not a good
/// idea.  Instead store the variables you want to use in the struct you derive from `Coroutine`.
///
/// ```
/// #include <cz/heap.hpp>
/// #include <cz/process.hpp>
/// #include <cz/string.hpp>
///
/// struct Coroutine_Read_File_To_String : Coroutine {
///     cz::Input_File file;
///     cz::String string;
///
///     bool tick();
/// };
///
/// bool Coroutine_Read_File_To_String::tick() {
///     CZ_CO_START;
///     while (1) {
///         CZ_CO_YIELD(false);
///
///         char buffer[1024];
///         int64_t result = file.read_binary(buffer, sizeof(buffer));
///         if (result > 0) {
///             // Process output.
///             string.reserve(cz::heap_allocator(), result);
///             string.append({buffer, (size_t)result});
///         } else if (result == 0) {
///             // Stop on end of file.
///             break;
///         } else {
///             // Ignore errors.
///         }
///     }
///     CZ_CO_END;
///     file.close();
///     return true;
/// }
///
/// Coroutine_Read_File_To_String coroutine = {};
/// if (coroutine.file.open("file_to_read")) {
///     while (1) {
///         if (coroutine.tick()) {
///             break;
///         }
///     }
///
///     // Use the string.
///     fputs("Read from file: ", stdout);
///     fwrite(string.buffer(), 1, string.len(), stdout);
///
///     string.drop(cz::heap_allocator());
/// }
/// ```

namespace cz {

struct Coroutine {
    long _state = 0;
};

#define CZ_CO_START   \
    switch (_state) { \
        case 0:
#define CZ_CO_END }

#define CZ_CO_YIELD(result) \
    do {                    \
        _state = __LINE__;  \
        return result;      \
        case __LINE__:;     \
    } while (0)

}
