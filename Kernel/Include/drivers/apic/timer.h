#include <mixins/utils/time.h>

#include <drivers/apic/device.h>
#include <quark/dev/device.h>
#include <quark/time/timer.h>

namespace Quark::Hal {
    class ApicTimerDevice
        : Time::Timer
        , Device
    {
    public:
        ApicTimerDevice(ApicDevice::Local* local)
            : Device("APIC Timer", Class::TimerOrClock)
            , m_local(local)
        {
        }

        void sleep(u64 ms) override;
        void sleep(Duration duration) override;
        // void sleepUntil(Date date) override;

    private:
        ApicDevice::Local* m_local;
        u64                m_clockFreq;
        volatile u64       m_clockTicks;
    };
} // namespace Quark::Hal