#include <drivers/smbios/spec.h>
#include <quark/dev/device.h>

namespace Quark::Hal {

    class SystemManagementBIOSDevice : public Io::Device
    {
    public:
        SystemManagementBIOSDevice();
        ~SystemManagementBIOSDevice() = default;

    private:
        u8    m_majorVer;
        u8    m_minorVer;
        void* m_smbiosInfo;
    };
} // namespace Quark::Hal