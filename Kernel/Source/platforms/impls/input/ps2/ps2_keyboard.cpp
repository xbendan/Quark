#include <drivers/ps2/device.h>

namespace PS2 {
    LegacyKeyboardDevice::LegacyKeyboardDevice()
        : Io::Device("PS/2 Legacy Keyboard Device", Device::Type::Keyboard)
    {
    }

    Res<> LegacyKeyboardDevice::OnStartup()
    {
        // Register IRQ handler for PS/2 Keyboard.
        return Ok();
    }
}