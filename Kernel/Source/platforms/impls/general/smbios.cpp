#include <drivers/smbios/device.h>
#include <quark/api/logging.h>
#include <quark/memory/address_range.h>
#include <quark/memory/address_space.h>

namespace Quark::System::Hal {
    const char* __signatureL2 = "_SM_";
    const char* __signatureL3 = "_SM3_";

    SystemManagementBIOSDevice::SystemManagementBIOSDevice()
        : Io::Device("System Management BIOS", Type::Firmware)
    {
        u64                  address = Memory::CopyAsIOAddress(0xF0000);
        Memory::AddressRange range(address, 0x10000);

        Optional<u64> result;
        if ((result = range.find(__signatureL2, 4, 0x10)).IsPresent()) {
            m_majorVer   = 2;
            m_smbiosInfo = (void*)result.Take();
            log(u8"[SMBIOS] Found SMBIOS 2.0 at %p\n", m_smbiosInfo);
        } else if ((result = range.find(__signatureL3, 5, 0x10)).IsPresent()) {
            m_majorVer   = 3;
            m_smbiosInfo = (void*)result.Take();
            log(u8"[SMBIOS] Found SMBIOS 3.0 at %p\n", m_smbiosInfo);
        }
    }
} // namespace Quark::System::Hal