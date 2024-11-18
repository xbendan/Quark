#include <drivers/acpi/device.h>
#include <drivers/acpi/spec.h>
#include <quark/dev/device.h>
#include <quark/timing/timer_source.h>

namespace ACPI {
    using namespace Quark::System;

    class ACPITimerDevice
        : public TimerSource
        , public Io::Device
    {
    public:
        ACPITimerDevice(ControllerDevice*);
        ~ACPITimerDevice() = default;

        Res<> OnInitialize() override;

        void            Sleep(u64) override;
        void            SleepNanos(u64) override;
        Res<TimerAlarm> CreateAlarm(Date, Func<void()>) override;
        Res<TimerAlarm> CreateAlarm(TimeSpan, Func<void()>) override;
        u64             GetSystemUptime() override;

    private:
        ACPI::ControllerDevice* m_controllerDevice;
        u8                      m_pmtTimerLength;
        AddressPack*            m_xpmtTimerBlock;
        bool                    m_useTimerExt;
        u16                     m_ticks;
    };
}