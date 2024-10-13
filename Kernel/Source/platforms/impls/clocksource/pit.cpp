#include <drivers/clocksource/pit/device.h>
#include <quark/dev/device.h>
#include <quark/hal/interrupts.h>
#include <quark/hal/ports.h>

namespace PIT {
    using namespace Quark::System::Hal;

    void PITimerDevice::Tick(InterruptStackFrame* frame)
    {
        m_instance->m_ticks++;
    }

    PITimerDevice::PITimerDevice(u32 frequency)
        : Device(u8"Programmable Interval Timer", Device::Type::TimerOrClock)
        , m_frequency(frequency)
    {
        m_instance = this;

        u32 divisor = PIT_NORMAL_FREQUENCY / frequency;
        m_commandAccess << (u8)0x36;
        m_dataAccess << (u8)(divisor & 0xFF);
        m_dataAccess << (u8)((divisor >> 8) & 0xFF);
    }

    Res<> PITimerDevice::OnStartup()
    {
        auto device = Device::FindByName<InterruptsControllerDevice>(
            "Interrupts Controller Device");

        if (!device.IsPresent()) {
            return Error::DeviceNotFound(
                "Interrupts Controller Device not found");
        }

        device.Take()->AddHandler(IRQ_PIT_TIMER_UPDATE, &PITimerDevice::Tick);
        return Ok();
    }

    Res<> PITimerDevice::OnShutdown()
    {
        auto device = Device::FindByName<InterruptsControllerDevice>(
            "Interrupts Controller Device");

        if (!device.IsPresent()) {
            return Error::DeviceNotFound(
                "Interrupts Controller Device not found");
        }

        device.Take()->RemoveHandler(IRQ_PIT_TIMER_UPDATE,
                                     &PITimerDevice::Tick);
        return Ok();
    }

    void PITimerDevice::Sleep(u64 ms)
    {
        assert(ms > 0);

        ms = ms * 1000 + m_uptime.load();
        while (m_uptime.load() < ms)
            asm volatile("pause");
    }

    void PITimerDevice::SleepNanos(u64 ms)
    {
        Sleep(ms / 1000000);
    }

    void PITimerDevice::SleepUntil(Date date)
    {
        u64 ms = (date - Date::Now()).ToMilliseconds();
        Sleep(ms);
    }

    Res<TimerAlarm> PITimerDevice::CreateAlarm(Date, Func<void()>) {}

    Res<TimerAlarm> PITimerDevice::CreateAlarm(TimeSpan, Func<void()>) {}

    Date PITimerDevice::Current() {}

}