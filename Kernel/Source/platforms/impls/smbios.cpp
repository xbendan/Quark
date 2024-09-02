#include <drivers/smbios/device.h>
#include <quark/api/logging.h>
#include <quark/memory/address_range.h>
#include <quark/memory/address_space.h>

namespace Quark::Hal {
    const char* __signatureL2 = "_SM_";
    const char* __signatureL3 = "_SM3_";

    SystemManagementBIOSDevice::SystemManagementBIOSDevice()
        : Io::Device("System Management BIOS", Class::Firmware)
    {
        u64               address = Mem::copyAsIOAddress(0xF0000);
        Mem::AddressRange range(address, 0x10000);

        Opt<u64> result;
        if ((result = range.find(__signatureL2, 4, 0x10)).isPresent()) {
            m_majorVer   = 2;
            m_smbiosInfo = (void*)result.take();
            log("[SMBIOS] Found SMBIOS 2.0 at %p\n", m_smbiosInfo);
        } else if ((result = range.find(__signatureL3, 5, 0x10)).isPresent()) {
            m_majorVer   = 3;
            m_smbiosInfo = (void*)result.take();
            log("[SMBIOS] Found SMBIOS 3.0 at %p\n", m_smbiosInfo);
        }
    }
} // namespace Quark::Hal