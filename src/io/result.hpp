#pragma once

namespace cz {
namespace io {

enum Result {
    Ok = 0,
    ErrorNotFound,
    ErrorPermissionDenied,
    ErrorConnectionRefused,
    ErrorConnectionReset,
    ErrorConnectionAborted,
    ErrorNotConnected,
    ErrorAddrInUse,
    ErrorAddrNotAvailable,
    ErrorBrokenPipe,
    ErrorAlreadyExists,
    ErrorWouldBlock,
    ErrorInvalidData,
    ErrorTimedOut,
    ErrorWriteZero,
    ErrorInterrupted,
    ErrorUnexpectedEof,
    ErrorOther,
};

}

inline bool is_err(io::Result r) {
    return r != io::Ok;
}

}
