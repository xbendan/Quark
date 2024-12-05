#include <drivers/ps2/device.h>
#include <quark/hal/interrupts.h>
#include <quark/hal/ports.h>
#include <quark/os/diagnostic/logging.h>

namespace PS2 {
    using namespace Quark::System::Hal;
    using namespace Quark::System::Diagnostic;

    static volatile bool UseShiftKeySet = false;

    bool OnKeyInterrupt(int, Registers* regs)
    {
        // while (in<byte>(PS2_COMMAND_PORT) & STATE_OUTPUT_BUFFER)
        //     return false;

        u8 resp    = in<byte>(PS2_DATA_PORT);
        u8 keyCode = resp & 0x7F;

        if (keyCode == 0xE0) {
            UseShiftKeySet = true;
            return true;
        }

        info$("Key: {}{}", (resp & 0x80 ? '+' : '-'), keyCode);

        return true;
    }

    LegacyKeyboardDevice::LegacyKeyboardDevice()
        : Io::Device("PS/2 Legacy Keyboard Device", Device::Type::Keyboard)
    {
        EnableInterrupt(IRQ_PS2_KEYBOARD, OnKeyInterrupt);
    }

    Res<> LegacyKeyboardDevice::OnInitialize()
    {
        // Register IRQ handler for PS/2 Keyboard.
        return Ok();
    }
}