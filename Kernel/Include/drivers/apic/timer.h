#pragma once

#include <drivers/apic/device.h>
#include <quark/dev/device.h>
#include <quark/timing/timer_source.h>

namespace APIC {
    class LocalAPICTimer : public TimerSource
    {
    public:
        LocalAPICTimer(GenericControllerDevice::Local*);
        ~LocalAPICTimer() = default;

        void            Sleep(u64) override;
        void            SleepNanos(u64) override;
        Res<TimerAlarm> CreateAlarm(Date, Func<void()>) override;
        Res<TimerAlarm> CreateAlarm(TimeSpan, Func<void()>) override;
        u64             GetSystemUptime() override;

    private:
        GenericControllerDevice::Local* m_local;
        u32                             m_busSpeed;
        u32                             m_irqSource;
    };
} // namespace Quark::System::Hal