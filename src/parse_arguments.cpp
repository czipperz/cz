#include <cz/parse_arguments.hpp>

#include <stdio.h>

namespace cz {

bool Arg_Parser::advance() {
    ++index;
    if (index == argc) {
        return false;
    }

    arg = argv[index];
    return true;
}

bool Arg_Parser::parse_string(cz::Str flag, cz::Str* out) {
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
    cz::append(&errors, "Error: ", flag, " requires either ", flag, " * or ", flag, "=*\n");
    return true;
}

bool Arg_Parser::parse_string_no_set(cz::Str flag, cz::Str* out) {
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
    cz::append(&errors, "Error: ", flag, " requires either ", flag, " * or ", flag, "*\n");
    return true;
}

bool Arg_Parser::parse_numeric(cz::Str flag, long* out) {
    cz::Str arg2;
    if (!parse_string(flag, &arg2)) {
        return false;
    }

    if (sscanf(arg2.buffer, "%ld", out) < 0) {
        cz::append(&errors, "Error: ", flag, " requires either ", flag, " * or ", flag, "=*\n");
    }

    return true;
}

bool Arg_Parser::parse_numeric_no_set(cz::Str flag, long* out) {
    cz::Str arg2;
    if (!parse_string_no_set(flag, &arg2)) {
        return false;
    }

    if (sscanf(arg2.buffer, "%ld", out) < 0) {
        cz::append(&errors, "Error: ", flag, " requires either ", flag, " * or ", flag, "*\n");
    }

    return true;
}

}
