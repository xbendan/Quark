#include <drivers/apic/device.h>

namespace Quark::Hal {
    void ApicDevice::Local::regWrite(u32 reg, u32 data)
    {
        *((volatile u32*)(m_baseVirt + reg)) = data;
    }

    u32 ApicDevice::Local::regRead(u32 reg)
    {
        return *((volatile u32*)(m_baseVirt + reg));
    }

    void ApicDevice::Local::sendIpi(u32 vec)
    {
        regWrite(LOCAL_APIC_ICR_HIGH, ((u32)m_apicId) << 24);
        regWrite(LOCAL_APIC_ICR_LOW, vec);

        while ((regRead((u32)LOCAL_APIC_ICR_LOW) & LOCAL_APIC_ICR_PENDING) != 0)
            ;
    }
} // namespace Quark::Hal