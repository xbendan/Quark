#include <mixins/utils/flags.h>

#define PS2_DATA_PORT 0x60
#define PS2_COMMAND_PORT 0x64

namespace PS2 {
    enum StateReg
    {
        STATE_OUTPUT_BUFFER   = 0x01,
        STATE_INPUT_BUFFER    = 0x02,
        STATE_SYSTEM_FLAG     = 0x04,
        STATE_COMMAND_DATA    = 0x08,
        STATE_KEYBOARD_LOCK   = 0x10,
        STATE_RECEIVE_TIMEOUT = 0x20,
        STATE_TIMEOUT_ERROR   = 0x40,
        STATE_PARITY_ERROR    = 0x80
    };

    enum class Command
    {
        READ_BYTE_ZERO         = 0x20,
        WRITE_BYTE_ZERO        = 0x60,
        DISABLE_SECOND_PORT    = 0xA7,
        ENABLE_SECOND_PORT     = 0xA8,
        TEST_SECOND_PORT       = 0xA9,
        TEST_PS2_CONTROLLER    = 0xAA,
        TEST_FIRST_PORT        = 0xAB,
        DIAGNOSTIC_DUMP        = 0xAC,
        DISABLE_FIRST_PORT     = 0xAD,
        ENABLE_FIRST_PORT      = 0xAE,
        READ_CONTROLLER_INPUT  = 0xC0,
        COPY_BITS_03_TO_47     = 0xC1,
        COPY_BITS_47_TO_47     = 0xC2,
        READ_CONTROLLER_OUTPUT = 0xD0
    };

    enum ConfigByte
    {
        FirstPortInterrupt   = 0x01,
        SecondPortInterrupt  = 0x02,
        SystemFlag           = 0x04,
        Reserved_0           = 0x08,
        FirstPortClock       = 0x10,
        SecondPortClock      = 0x20,
        FirstPortTranslation = 0x40,
        Reserved_1           = 0x80
    };

} // namespace Quark::System::Hal::PS2