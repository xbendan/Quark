#include <drivers/clocksource/pit/spec.h>
#include <mixins/concurrent/atomic.h>
#include <mixins/utils/list.h>
#include <quark/dev/device.h>
#include <quark/hal/interrupts.h>
#include <quark/hal/ports.h>
#include <quark/timing/timer_source.h>

namespace PIT {
    using Qk::List;
    using namespace Quark::System;
    using namespace Quark::System::Hal;

    class PITimerDevice
        : public TimerSource
        , public Io::Device
    {
    public:
        PITimerDevice(u32 frequency);

        static bool Tick(int, Registers* frame);

        virtual Res<> OnInitialize() override final;
        virtual Res<> OnStartup() override final;
        virtual Res<> OnShutdown() override final;

        virtual void            Sleep(u64) override final;
        virtual void            SleepNanos(u64) override final;
        virtual Res<TimerAlarm> CreateAlarm(Date, Func<void()>) override final;
        virtual Res<TimerAlarm> CreateAlarm(TimeSpan,
                                            Func<void()>) override final;
        virtual u64             GetSystemUptime() override final;

    private:
        Atomic<u64>                         m_uptime{ 0 };
        List<TimerAlarm>                    m_alarms{};
        u32                                 m_frequency;
        Hal::PortAccess<PIT_DATA_CHANNEL_0> m_dataAccess;
        Hal::PortAccess<PIT_COMMAND_REG>    m_commandAccess;

        static inline PITimerDevice* m_instance;
    };
}