#include <drivers/smbios/device.h>
#include <quark/memory/address_range.h>
#include <quark/memory/address_space.h>
#include <quark/os/diagnostic/logging.h>

namespace Quark::System::Hal {
    using namespace Quark::System::Diagnostic;

    const char* __signatureL2 = "_SM_";
    const char* __signatureL3 = "_SM3_";

    SystemManagementBIOSDevice::SystemManagementBIOSDevice()
        : Io::Device("System Management BIOS", Type::Firmware)
    {
        u64                  address = Memory::CopyAsIOAddress(0xF0000);
        Memory::AddressRange range(address, 0x10000);

        Optional<u64> result;
        if ((result = range.Find(__signatureL2, 4, 0x10)).IsPresent()) {
            m_majorVer   = 2;
            m_smbiosInfo = (void*)result.Take();
            info$("[SMBIOS] Found SMBIOS 2.0 at {#X}", m_smbiosInfo);
        } else if ((result = range.Find(__signatureL3, 5, 0x10)).IsPresent()) {
            m_majorVer   = 3;
            m_smbiosInfo = (void*)result.Take();
            info$("[SMBIOS] Found SMBIOS 3.0 at {#X}", m_smbiosInfo);
        }
    }
} // namespace Quark::System::Hal