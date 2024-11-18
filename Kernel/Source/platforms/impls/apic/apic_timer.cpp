#include <drivers/apic/interrupt.h>
#include <drivers/apic/timer.h>
#include <quark/sched/completable.h>

namespace APIC {
    LocalAPICTimer::LocalAPICTimer(GenericControllerDevice::Local* local)
        : m_local(local)
        , m_irqSource(0x20)
    {
        local->RegWrite(LOCAL_APIC_LVT_TIMER, 0x10000);
        local->RegWrite(LOCAL_APIC_TIMER_DIVIDE, 0x3);
        local->RegWrite(LOCAL_APIC_LVT_TIMER, 0x0);
        u32 t = 0xffffffff;
        local->RegWrite(LOCAL_APIC_TIMER_INITIAL_COUNT, t);

        Task::Delay(100);
        local->RegWrite(LOCAL_APIC_LVT_TIMER, 0x10000);

        m_busSpeed = (t - local->RegRead(LOCAL_APIC_TIMER_CURRENT_COUNT)) * 10;
        m_busSpeed *= 10;

        local->RegWrite(LOCAL_APIC_LVT_TIMER, 0x00020000 | m_irqSource);
        // Divide by 16
        local->RegWrite(LOCAL_APIC_TIMER_DIVIDE, 0x3);
        local->RegWrite(LOCAL_APIC_TIMER_INITIAL_COUNT,
                        (m_busSpeed / 0x10) / 0x1000);

        EnableInterrupt(m_irqSource);
    }

    void LocalAPICTimer::Sleep(u64 ms)
    {
        // u64 ticks = ms * m_busSpeed / 1000;
        // m_local->RegWrite(LOCAL_APIC_TIMER_INITIAL_COUNT, ticks);
        // while (m_local->RegRead(LOCAL_APIC_TIMER_CURRENT_COUNT) > 0)
        //     ;
    }

    void LocalAPICTimer::SleepNanos(u64 ns) {}

    Res<TimerAlarm> LocalAPICTimer::CreateAlarm(Date         date,
                                                Func<void()> callback)
    {
        return Error::NotImplemented();
    }

    Res<TimerAlarm> LocalAPICTimer::CreateAlarm(TimeSpan     span,
                                                Func<void()> callback)
    {
        return Error::NotImplemented();
    }

    u64 LocalAPICTimer::GetSystemUptime()
    {
        return 0;
    }
}