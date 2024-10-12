#include <drivers/pci/device.h>
#include <drivers/storage/ahci/spec.h>
#include <mixins/utils/linked_list.h>
#include <quark/dev/builtin_devices.h>

namespace Quark::System::Hal {
    class AHCIControllerDevice;

    class SATADiskDevice : public Io::DiskDevice
    {
    public:
        SATADiskDevice(int                   port,
                       AHCI::HBAPortRegs*    portRegs,
                       AHCIControllerDevice* controller);
        ~SATADiskDevice() override = default;

        i64 Read(u64 offset, u64 size, void* buffer) override;
        i64 Write(u64 offset, u64 size, void* buffer) override;

        void startCommand();
        void stopCommand();

        int status() { return m_status; }

        int access(u64 lba, u32 count, u64 physBuffer, int write);

        int findSlot();

    private:
        AHCI::HBAPortRegs*      m_portRegs;
        AHCI::HBAMemRegs*       m_memRegs;
        AHCI::HBACommandHeader* m_commandList;
        AHCI::HBACommandTable*  m_commandTable[8];
        AHCI::HBAFrameInfo*     m_fis;
        int                     m_status;

        u64 m_physBuffer[8];
    };

    class AHCIControllerDevice : public PCI::PCIDevice
    {
    public:
        AHCIControllerDevice(PCIInfo& info);
        ~AHCIControllerDevice();

        AHCI::HBAMemRegs* memRegs() { return m_memRegs; }

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
} // namespace Quark::System::Hal
