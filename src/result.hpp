#pragma once

namespace cz {

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

inline bool is_err(Result r) {
    return r != Ok;
}

}
