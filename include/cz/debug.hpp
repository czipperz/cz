#pragma once

namespace cz {

/// If the program is being debugged then break.
void dbreak();

/// If the `condition == true` then calls `dbreak`.
void cbreak(bool condition);

}
