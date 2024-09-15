#include <drivers/apic/device.h>

namespace APIC {
    void GenericControllerDevice::Local::regWrite(u32 reg, u32 data)
    {
        *((volatile u32*)(_baseVirt + reg)) = data;
    }

    u32 GenericControllerDevice::Local::regRead(u32 reg)
    {
        return *((volatile u32*)(_baseVirt + reg));
    }

    void GenericControllerDevice::Local::setEnabled()
    {
        regWrite(LOCAL_APIC_SIVR, 0x1FF);
    }

    void GenericControllerDevice::Local::callIPI(u32 vec)
    {
        regWrite(LOCAL_APIC_ICR_HIGH, ((u32)_apicId) << 24);
        regWrite(LOCAL_APIC_ICR_LOW, vec);

        while ((regRead((u32)LOCAL_APIC_ICR_LOW) & LOCAL_APIC_ICR_PENDING) != 0)
            ;
    }

    void GenericControllerDevice::Local::callIPI(u32 dsh, u32 type, u8 vector)
    {
        callIPI(dsh | type | ICR_VECTOR(vector));
    }
} // namespace Quark::System::Hal