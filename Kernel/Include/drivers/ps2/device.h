#include <drivers/ps2/spec.h>
#include <quark/dev/device.h>

namespace PS2 {
    using namespace Quark::System;

    class LegacyKeyboardDevice : public Io::Device
    {
    public:
        LegacyKeyboardDevice();
        ~LegacyKeyboardDevice() = default;

        Res<> OnStartup() override;
        Res<> OnShutdown() override;
    };

    class LegacyMouseDevice : public Io::Device
    {};

    class LegacyControllerDevice : public Io::Device
    {
    public:
        LegacyControllerDevice();
        ~LegacyControllerDevice();

        Res<> OnInitialize() override;

        void send(PS2::Command);
        void flush();
        void wait();

    private:
        LegacyKeyboardDevice* m_keyboard;
        LegacyMouseDevice*    m_mouse;
    };
} // namespace Quark::System::Hal