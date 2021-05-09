#pragma once

#include "format.hpp"
#include "heap_string.hpp"
#include "str.hpp"

namespace cz {

/// Parse command line arguments to your app.  The interface here is
/// intentionally designed to be really simple and easy to extend.
///
/// The parse functions return `true` if the `flag` was matched.  They will *not* set `out`
/// unless `*` is specified.  This means that they will return `true` without setting `out`
/// in some cases.  Due to this every config variable must have a default value.
///
/// For example: given arguments `["--flag"]` and `parse_string("--flag", &flag_value)`
/// is called, `true` will be returned but `flag_value` will not be modified.
///
/// Example:
/// ```
/// // Note: config variables must have default values!
/// cz::Str config = "default";
/// long timeout = 1000;
/// long num = 32;
/// cz::Heap_Vector<cz::Str> generic_arguments = {};
/// CZ_DEFER(generic_arguments.drop());
///
/// Arg_Parser parser = {};
/// CZ_DEFER(parser.drop());
/// parser.argc = argc;
/// parser.argv = argv;
/// bool show_help = false;
/// while (parser.advance()) {
///     if (parser.arg == "--help" || parser.arg == "-?" || parser.arg == "/?") {
///         // Show help but don't raise an error.
///         show_help = true;
///     } else if (parser.parse_string("--config", &config)) {
///         // --config * or --config=*
///     } else if (parser.parse_numeric("--timeout", &timeout)) {
///         // --timeout * or --timeout=*
///     } else if (parser.parse_numeric_no_set("-n", &num)) {
///         // -n * or -n*
///     } else if (parser.parse_numeric("--num", &num)) {
///         // --num * or --num=*
///     } else {
///         generic_arguments.reserve(1);
///         generic_arguments.push(parser.arg);
///     }
/// }
///
/// if (parser.errors.len() > 0) {
///     fwrite(parser.errors.buffer(), 1, parser.errors.len(), stderr);
///     putc('\n', stderr);
///     show_help = true;
/// }
///
/// if (show_help) {
///     fprintf(stderr,
///             "Options:\n\
/// --timeout TIMEOUT or --timeout=TIMEOUT\n\
///     Specify a timeout in milliseconds.  Defaults to 1000.\n\
/// --config CONFIG or --config=CONFIG\n\
///     Specify which configuration to use.  Defaults to default.\n\
/// -n NUM or -nNUM or --num NUM or --num=NUM\n\
///     Specify the number of things.  Defaults to 32.\n");
///     return parser.errors.len() > 0;
/// }
/// ```
struct Arg_Parser {
    int argc;
    char** argv;
    int index;
    cz::Str arg;

    cz::Heap_String errors;

    void drop() { errors.drop(); }

    /// Go to the next argument.  Returns `false` if there is no next argument.
    bool advance();

    /// Parse `flag *` or `flag=*`.
    bool parse_string(cz::Str flag, cz::Str* out);

    /// Parse `flag *` or `flag*`.
    bool parse_string_no_set(cz::Str flag, cz::Str* out);

    /// Parse `flag *` or `flag=*` and parse `*` as a number.
    bool parse_numeric(cz::Str flag, long* out);

    /// Parse `flag *` or `flag*` and parse `*` as a number.
    bool parse_numeric_no_set(cz::Str flag, long* out);
};

}
