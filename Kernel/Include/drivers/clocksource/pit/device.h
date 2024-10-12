#include <drivers/clocksource/pit/spec.h>
#include <quark/clocksource/timer.h>
#include <quark/dev/device.h>
#include <quark/hal/definition.h>
#include <quark/hal/ports.h>

namespace PIT {
    using namespace Quark::System;
    using namespace Quark::System::Hal;

    class PITimerDevice
        : public Timer
        , public Io::Device
    {
    public:
        PITimerDevice(u32 frequency);

        static void Tick(InterruptStackFrame* frame);

        virtual Res<> OnStartup() override final;

        virtual void            Sleep(u64) override final;
        virtual void            SleepNanos(u64) override final;
        virtual void            SleepUntil(Date) override final;
        virtual Res<TimerAlarm> CreateAlarm(Date, Func<void()>) override final;
        virtual Res<TimerAlarm> CreateAlarm(TimeSpan,
                                            Func<void()>) override final;
        virtual Date            Current() override final;

    private:
        u64                                 m_ticks;
        u32                                 m_frequency;
        Hal::PortAccess<PIT_DATA_CHANNEL_0> m_dataAccess;
        Hal::PortAccess<PIT_COMMAND_REG>    m_commandAccess;

        static inline PITimerDevice* m_instance;
    };
}