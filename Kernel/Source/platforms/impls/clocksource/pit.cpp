#include <drivers/clocksource/pit/device.h>
#include <quark/dev/device.h>
#include <quark/hal/interrupts.h>
#include <quark/hal/ports.h>
#include <quark/os/diagnostic/logging.h>

namespace PIT {
    using namespace Quark::System::Hal;
    using namespace Quark::System::Diagnostic;

    bool PITimerDevice::Tick(int, Registers* frame)
    {
        m_instance->m_uptime.Inc(MemoryOrder::MemoryOrderRelaxed);

        return true;
    }

    PITimerDevice::PITimerDevice(u32 frequency)
        : TimerSource()
        , Device("Programmable Interval Timer", Device::Type::TimerOrClock)
        , m_frequency(frequency)
    {
        m_instance = this;
    }

    Res<> PITimerDevice::OnInitialize()
    {
        EnableInterrupt(IRQ_PIT_TIMER_UPDATE, PITimerDevice::Tick);

        u32 divisor = PIT_NORMAL_FREQUENCY / m_frequency;

        m_commandAccess << (u8)0x36;
        m_dataAccess << (u8)(divisor & 0xFF);
        m_dataAccess << (u8)((divisor >> 8) & 0xFF);

        return Ok();
    }

    Res<> PITimerDevice::OnStartup()
    {
        return Ok();
    }

    Res<> PITimerDevice::OnShutdown()
    {
        EnableInterrupt(IRQ_PIT_TIMER_UPDATE, nullptr);
        return Ok();
    }

    void PITimerDevice::Sleep(u64 ms)
    {
        assert(ms > 0);

        ms = ms * 1000 + m_uptime.Load();
        while (m_uptime.Load() < ms)
            asm volatile("hlt");
    }

    void PITimerDevice::SleepNanos(u64 ms)
    {
        Sleep(ms / 1000000);
    }

    Res<TimerAlarm> PITimerDevice::CreateAlarm(Date, Func<void()>)
    {
        return Error::NotImplemented();
    }

    Res<TimerAlarm> PITimerDevice::CreateAlarm(TimeSpan, Func<void()>)
    {
        return Error::NotImplemented();
    }

    u64 PITimerDevice::GetSystemUptime()
    {
        return m_uptime.Load();
    }
}