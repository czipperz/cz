#include <cz/dwim/process.hpp>

#include <cz/defer.hpp>
#include <cz/format.hpp>

namespace cz {
namespace dwim {

bool read_to_string(Dwim* dwim, Input_File* file, String* output) {
    Carriage_Return_Carry carry;
    while (1) {
        int64_t result =
            file->read_text((char*)dwim->temp_buffer.buffer, dwim->temp_buffer.size, &carry);
        if (result > 0) {
            output->reserve(dwim->buffer_array.allocator(), result);
            output->append({(char*)dwim->temp_buffer.buffer, (size_t)result});
        } else if (result == 0) {
            output->realloc(dwim->buffer_array.allocator());
            return true;
        } else if (result < 0) {
            output->realloc(dwim->buffer_array.allocator());
            return false;
        }
    }
}

String read_file(Dwim* dwim, const char* path) {
    String output = {};

    // Open the file.
    Input_File file;
    if (!file.open(path)) {
        dwim->errors.reserve(1);
        dwim->errors.push(format(dwim->buffer_array.allocator(), "Couldn't open file ", path));
        return output;
    }
    CZ_DEFER(file.close());

    // Read the contents.
    if (!read_to_string(dwim, &file, &output)) {
        dwim->errors.reserve(1);
        dwim->errors.push(
            format(dwim->buffer_array.allocator(), "Error reading file '", path, "'"));
    }

    return output;
}

String run_script(Dwim* dwim, const char* script) {
    String output = {};

    Process_Options options;

    // Open pipe so we can read output from the process.
    Input_File file;
    if (!create_process_output_pipe(&options.std_out, &file)) {
        dwim->errors.reserve(1);
        dwim->errors.push(format(dwim->buffer_array.allocator(),
                                 "Error creating pipe when starting script '", script, "'"));
        return output;
    }
    CZ_DEFER(file.close());

    // Merge stderr and stdout streams.
    options.std_err = options.std_out;

    // Start the process.
    Process process;
    if (!process.launch_script(script, &options)) {
        dwim->errors.reserve(1);
        dwim->errors.push(
            format(dwim->buffer_array.allocator(), "Error starting script '", script, "'"));
        return output;
    }
    CZ_DEFER(process.join());

    // Read the output.
    if (!read_to_string(dwim, &file, &output)) {
        dwim->errors.reserve(1);
        dwim->errors.push(format(dwim->buffer_array.allocator(), "Error reading output of script '",
                                 script, "'"));
    }

    return output;
}

}
}
