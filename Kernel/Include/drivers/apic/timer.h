#include <drivers/apic/device.h>
#include <quark/dev/device.h>
#include <quark/time/clocksource.h>

namespace APIC {
    class APICTimerDevice
        : public System::IClockSource
        , public Io::Device
    {
    public:
        APICTimerDevice(GenericControllerDevice::Local* local)
            : Device(u8"APIC Timer", Class::TimerOrClock)
            , m_local(local)
            , m_clockFreq(0)
            , m_clockTicks(0)
        {
        }

        u64  getTicks() override;
        Date now() override;
        void sleep(u64 ms) override;
        void sleepUntil(Date date) override;

    private:
        GenericControllerDevice::Local* m_local;
        u64                             m_clockFreq;
        volatile u64                    m_clockTicks;
    };
} // namespace Quark::System::Hal