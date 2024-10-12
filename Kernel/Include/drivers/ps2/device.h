#include <drivers/ps2/spec.h>
#include <quark/dev/device.h>
#include <quark/hal/ports.h>

namespace PS2 {
    using namespace Quark::System;

    class LegacyKeyboardDevice : public Io::Device
    {
    public:
        LegacyKeyboardDevice();
        ~LegacyKeyboardDevice() = default;

        Res<> OnStartup() override;
    };

    class LegacyMouseDevice : public Io::Device
    {};

    class LegacyControllerDevice : public Io::Device
    {
    public:
        LegacyControllerDevice();
        ~LegacyControllerDevice();

        Res<> OnInitialize() override;

        void Send(PS2::Command);
        void Flush();
        void Wait();

    private:
        LegacyKeyboardDevice*             m_keyboard;
        LegacyMouseDevice*                m_mouse;
        Hal::PortAccess<PS2_DATA_PORT>    m_dataAccess;
        Hal::PortAccess<PS2_COMMAND_PORT> m_commandAccess;
    };
} // namespace Quark::System::Hal