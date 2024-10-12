#include <drivers/pci/device.h>
#include <drivers/storage/ahci/disk_device.h>
#include <drivers/storage/ahci/spec.h>

namespace AHCI {
    class AHCIControllerDevice : public PCI::PCIDevice
    {
    public:
        AHCIControllerDevice(PCIInfo& info);
        ~AHCIControllerDevice();

        AHCI::HBAMemRegs* GetMemoryRegs() { return m_memRegs; }

        u64 clbPhys() { return m_clbPhys; }

        u64 fbPhys() { return m_fbPhys; }

        u64 ctbaPhys() { return m_ctbaPhys; }

    private:
        u64               m_clbPhys, m_fbPhys, m_ctbaPhys;
        u64               m_addrBase;
        u64               m_addrVirt;
        AHCI::HBAMemRegs* m_memRegs;

        LinkedList<SATADiskDevice*> m_diskList;
    };
}