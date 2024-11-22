#pragma once

struct [[nodiscard]] Error
{
    enum class Code
    {
        // Success
        SUCCESS = 0,

        // General errors
        NULL_REFERENCE,
        INVALID_ARGUMENT,
        INVALID_OPERATION,
        INVALID_STATE,
        NOT_IMPLEMENTED,
        OUT_OF_MEMORY,
        INDEX_OUT_OF_BOUNDS,
        TIMED_OUT,

        // Memory related errors
        ADDRESS_EXCEEDS_LIMIT,
        PAGE_NOT_EXIST,
        PAGE_NOT_PRESENT,
        PAGE_NOT_WRITABLE,
        PAGE_NOT_EXECUTABLE,
        PAGE_NOT_USER,
        PAGE_NOT_FREE,
        PAGE_MAY_BROKEN,
        ALLOCATE_FAILED,

        // Device related errors
        DEVICE_NOT_FOUND,
        DEVICE_FAULT,
        DEVICE_DUPLICATED,

        // File related errors
        FILE_NOT_FOUND,
        FILE_ALREADY_EXISTS,
        FILE_LOCKED,
        FILE_NOT_READABLE,
        FILE_NOT_WRITABLE,
        FILE_NOT_EXECUTABLE,
        FILE_TYPE_MISMATCH,

        // Network related errors
        NETWORK_ERROR,
        NETWORK_TIMEOUT,
        NETWORK_DISCONNECTED,
        NETWORK_PROTOCOL_ERROR,
        NETWORK_ADDRESS_IN_USE,
        NETWORK_ADDRESS_NOT_AVAILABLE,
        NETWORK_CONNECTION_REFUSED,
        NETWORK_CONNECTION_RESET,
        NETWORK_CONNECTION_ABORTED,
        NETWORK_CONNECTION_CLOSED,

        // System related errors
        SYSTEM_ERROR,
        SYSTEM_SHUTDOWN,

        // Unknown error
        UNKNOWN
    } _code;
    const char* _message;

    // using enum Code;

    constexpr Error()
        : _code(Code::UNKNOWN)
        , _message("")
    {
    }

    constexpr Error(Code code, const char* message)
        : _code(code)
        , _message(message)
    {
    }

    constexpr Code code() const { return _code; }

    constexpr const char* message() const { return _message; }

    constexpr bool operator==(const Error& other) const
    {
        return _code == other._code;
    }

#define MakeError$(name, code, _msg)                                           \
    static Error name(const char* msg = _msg) { return { Code::code, msg }; }

    MakeError$(NullReference, NULL_REFERENCE, "Null object reference")
        MakeError$(InvalidArgument, INVALID_ARGUMENT, "Invalid argument");
    MakeError$(InvalidState, INVALID_STATE, "Invalid state");
    MakeError$(InvalidOperation, INVALID_OPERATION, "Invalid operation");
    MakeError$(NotImplemented,
               NOT_IMPLEMENTED,
               "This method is not implemented yet.");
    MakeError$(OutOfMemory, OUT_OF_MEMORY, "Out of memory");
    MakeError$(IndexOutOfBounds, INDEX_OUT_OF_BOUNDS, "Index out of bounds");
    MakeError$(TimedOut, TIMED_OUT, "Timed out");
    MakeError$(AddressExceedsLimit,
               ADDRESS_EXCEEDS_LIMIT,
               "Address exceeds limit");
    MakeError$(PageNotExist, PAGE_NOT_EXIST, "Page does not exist");
    MakeError$(PageNotPresent, PAGE_NOT_PRESENT, "Page is not present");
    MakeError$(PageNotWritable, PAGE_NOT_WRITABLE, "Page is not writable");
    MakeError$(PageNotExecutable,
               PAGE_NOT_EXECUTABLE,
               "Page is not executable");
    MakeError$(PageNotUser, PAGE_NOT_USER, "Page is not user");
    MakeError$(PageNotFree, PAGE_NOT_FREE, "Page is not free");
    MakeError$(PageMayBroken, PAGE_MAY_BROKEN, "Page may be broken");
    MakeError$(AllocateFailed, ALLOCATE_FAILED, "Failed to allocate memory");
    MakeError$(DeviceNotFound, DEVICE_NOT_FOUND, "Device not found");
    MakeError$(DeviceFault, DEVICE_FAULT, "Device fault");
    MakeError$(DeviceDuplicated, DEVICE_DUPLICATED, "Device duplicated");
    MakeError$(FileNotFound, FILE_NOT_FOUND, "File not found");
    MakeError$(FileAlreadyExists, FILE_ALREADY_EXISTS, "File already exists");
    MakeError$(FileLocked, FILE_LOCKED, "File is locked");
    MakeError$(FileNotReadable, FILE_NOT_READABLE, "File is not readable");
    MakeError$(FileNotWritable, FILE_NOT_WRITABLE, "File is not writable");
    MakeError$(FileNotExecutable,
               FILE_NOT_EXECUTABLE,
               "File is not executable");
    MakeError$(FileTypeMismatch, FILE_TYPE_MISMATCH, "File type mismatch");
    MakeError$(NetworkError, NETWORK_ERROR, "Network error");
    MakeError$(NetworkTimeout, NETWORK_TIMEOUT, "Network timeout");
    MakeError$(NetworkDisconnected,
               NETWORK_DISCONNECTED,
               "Network disconnected");
    MakeError$(NetworkProtocolError,
               NETWORK_PROTOCOL_ERROR,
               "Network protocol error");
    MakeError$(NetworkAddressInUse, NETWORK_ADDRESS_IN_USE, "Address in use");
    MakeError$(NetworkAddressNotAvailable,
               NETWORK_ADDRESS_NOT_AVAILABLE,
               "Address not available");
    MakeError$(NetworkConnectionRefused,
               NETWORK_CONNECTION_REFUSED,
               "Connection refused");
    MakeError$(NetworkConnectionReset,
               NETWORK_CONNECTION_RESET,
               "Connection reset");
    MakeError$(NetworkConnectionAborted,
               NETWORK_CONNECTION_ABORTED,
               "Connection aborted");
    MakeError$(NetworkConnectionClosed,
               NETWORK_CONNECTION_CLOSED,
               "Connection closed");
    MakeError$(SystemError, SYSTEM_ERROR, "System error");
    MakeError$(SystemShutdown, SYSTEM_SHUTDOWN, "System shutdown");
};

namespace Qk {
    struct Exception
    {
        const char* _message;
    };
}