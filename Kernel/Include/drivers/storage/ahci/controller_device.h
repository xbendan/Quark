#include <drivers/pci/device.h>
#include <drivers/storage/ahci/spec.h>
#include <drivers/storage/ahci/storage_device.h>

namespace AHCI {
    using Qk::List;

    class AHCIStorageControllerDevice : public PCI::PCIDevice
    {
    public:
        AHCIStorageControllerDevice(PCIInfo& info);
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