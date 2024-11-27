#include <drivers/pci/device.h>
#include <drivers/storage/ahci/ahci_storage_device.h>
#include <drivers/storage/ahci/spec.h>

namespace AHCI {
    using Qk::List;

    class AHCIStorageControllerDevice : public PCI::Device
    {
    public:
        AHCIStorageControllerDevice(DeviceIdentifier& info);
        ~AHCIStorageControllerDevice();

        AHCI::HBAMemRegs* GetMemoryRegs() { return m_memRegs; }

    protected:
        friend class SATAStorageDevice;

        u64               m_clbPhys, m_fbPhys, m_ctbaPhys;
        u64               m_addrPhys;
        u64               m_addrVirt;
        AHCI::HBAMemRegs* m_memRegs;

        List<SATAStorageDevice*> m_diskList;
    };
}