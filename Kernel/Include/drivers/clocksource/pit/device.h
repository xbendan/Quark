#include <drivers/clocksource/pit/spec.h>
#include <mixins/concurrent/atomic.h>
#include <mixins/utils/list.h>
#include <quark/clocksource/timer.h>
#include <quark/dev/device.h>
#include <quark/hal/interrupts.h>
#include <quark/hal/ports.h>

namespace PIT {
    using Qk::List;
    using namespace Quark::System;
    using namespace Quark::System::Hal;

    class PITimerDevice
        : public Timer
        , public Io::Device
    {
    public:
        PITimerDevice(u32 frequency);

        static bool Tick(int, Registers* frame);

        virtual Res<> OnStartup() override final;
        virtual Res<> OnShutdown() override final;

        virtual void            Sleep(u64) override final;
        virtual void            SleepNanos(u64) override final;
        virtual void            SleepUntil(Date) override final;
        virtual Res<TimerAlarm> CreateAlarm(Date, Func<void()>) override final;
        virtual Res<TimerAlarm> CreateAlarm(TimeSpan,
                                            Func<void()>) override final;
        virtual Date            GetToday() override final;
        virtual u64             GetSystemUptime() override final;
        virtual u64             GetTimestamp() override final;

    private:
        Atomic<u64>                         m_uptime{ 0 };
        List<TimerAlarm>                    m_alarms{};
        u32                                 m_frequency;
        Hal::PortAccess<PIT_DATA_CHANNEL_0> m_dataAccess;
        Hal::PortAccess<PIT_COMMAND_REG>    m_commandAccess;

        static inline PITimerDevice* m_instance;
    };
}