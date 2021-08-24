#include <cz/parse_arguments.hpp>

#include <stdio.h>
#include <cz/parse.hpp>

namespace cz {

bool Arg_Parser::advance() {
    ++index;
    if (index == argc) {
        return false;
    }

    arg = argv[index];
    return true;
}

bool Arg_Parser::parse_string(Str flag, Str* out) {
    if (!arg.starts_with(flag)) {
        return false;
    }

    if (arg.len == flag.len) {
        if (index + 1 == argc) {
            goto error;
        }

        *out = argv[index + 1];
        ++index;
    } else {
        if (arg[flag.len] != '=') {
            goto error;
        }

        *out = arg.slice_start(flag.len + 1);
    }

    return true;

error:
    append(&errors, "Error: ", flag, " requires either ", flag, " * or ", flag, "=*\n");
    return true;
}

bool Arg_Parser::parse_string_no_set(Str flag, Str* out) {
    if (!arg.starts_with(flag)) {
        return false;
    }

    if (arg.len == flag.len) {
        if (index + 1 == argc) {
            goto error;
        }

        *out = argv[index + 1];
        ++index;
    } else {
        *out = arg.slice_start(flag.len);
    }

    return true;

error:
    append(&errors, "Error: ", flag, " requires either ", flag, " * or ", flag, "*\n");
    return true;
}

bool Arg_Parser::parse_numeric(Str flag, int64_t* out) {
    Str arg2;
    if (!parse_string(flag, &arg2)) {
        return false;
    }

    if (parse(arg2, out) < 0) {
        append(&errors, "Error: ", flag, " requires either ", flag, " * or ", flag, "=*\n");
    }

    return true;
}

bool Arg_Parser::parse_numeric_no_set(Str flag, int64_t* out) {
    Str arg2;
    if (!parse_string_no_set(flag, &arg2)) {
        return false;
    }

    if (parse(arg2, out) < 0) {
        append(&errors, "Error: ", flag, " requires either ", flag, " * or ", flag, "*\n");
    }

    return true;
}

}
