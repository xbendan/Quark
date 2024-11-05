#include <drivers/apic/device.h>

namespace APIC {
    void GenericControllerDevice::Local::RegWrite(u32 reg, u32 data)
    {
        *((volatile u32*)(_baseVirt + reg)) = data;
    }

    u32 GenericControllerDevice::Local::RegRead(u32 reg)
    {
        return *((volatile u32*)(_baseVirt + reg));
    }

    void GenericControllerDevice::Local::Enable()
    {
        RegWrite(LOCAL_APIC_SIVR, 0x1FF);
    }

    void GenericControllerDevice::Local::SendIPI(u32 vec)
    {
        RegWrite(LOCAL_APIC_ICR_HIGH, ((u32)_apicId) << 24);
        RegWrite(LOCAL_APIC_ICR_LOW, vec);

        while ((RegRead((u32)LOCAL_APIC_ICR_LOW) & LOCAL_APIC_ICR_PENDING) != 0)
            ;
    }

    void GenericControllerDevice::Local::SendIPI(u32 dsh, u32 type, u8 vector)
    {
        SendIPI(dsh | type | ICR_VECTOR(vector));
    }
} // namespace Quark::System::Hal