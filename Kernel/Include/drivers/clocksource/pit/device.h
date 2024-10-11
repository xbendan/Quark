#include <drivers/clocksource/pit/spec.h>
#include <quark/clocksource/timer.h>
#include <quark/dev/device.h>
#include <quark/hal/ports.h>

namespace PIT {
    using namespace Quark::System;

    class PITimerDevice
        : public Timer
        , public Io::Device
    {
    public:
        PITimerDevice(u32 frequency);

        virtual void Sleep(u64) override final;
        virtual void SleepNanos(u64) override final;
        virtual void SleepUntil(Date) override final;
        virtual Date Current() override final;

    private:
        u32                                 m_frequency;
        Hal::PortAccess<PIT_DATA_CHANNEL_0> m_dataAccess;
        Hal::PortAccess<PIT_COMMAND_REG>    m_commandAccess;
    };
}