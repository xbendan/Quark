#include <drivers/storage/ahci/device.h>
#include <quark/memory/address_space.h>
#include <quark/memory/page_alloc.h>

namespace Quark::System::Hal {
    using namespace Quark::System::Memory;

    AHCIControllerDevice::AHCIControllerDevice(PCIInfo& info)
        : PCIDevice(info, "AHCI Controller Device", Type::DiskController)
        , m_clbPhys(AllocatePhysMemory4K(8).Unwrap())
        , m_fbPhys(AllocatePhysMemory4K(2).Unwrap())
        , m_ctbaPhys(AllocatePhysMemory4K(64).Unwrap())
        , m_addrBase(getBaseAddrRegs(5))
    {
        enableBusMastering();
        enableInterrupts();
        enableMemorySpace();

        m_addrVirt = Memory::copyAsIOAddress(m_addrBase);
        m_memRegs  = reinterpret_cast<AHCI::HBAMemRegs*>(m_addrVirt);

        m_memRegs->_ghc &= ~AHCI_GHC_IE;
        m_memRegs->_interruptStatus = 0xFFFFFFFF;

        for (int i = 0; i < 32; i++) {
            if (!(m_memRegs->_portsImplemented & (1 << i))) {
                continue;
            }

            AHCI::HBAPortRegs* portRegs = &m_memRegs->_ports[i];
            u32                ssts     = portRegs->_ssts;
            u8                 ipm = (ssts >> 8) & 0x0F, det = ssts & 0x0F;
            if (ipm != HBA_PxSSTS_DET_PRESENT || det != HBA_PORT_IPM_ACTIVE) {
                continue;
            }

            switch (portRegs->_sig) {
                case SATA_SIG_ATAPI: {
                    break;
                }
                case SATA_SIG_SEMB: {
                    break;
                }
                case SATA_SIG_PM: {
                    break;
                }
                default: {
                    SATADiskDevice* disk =
                        new SATADiskDevice(i, portRegs, this);

                    if (disk->status() != 1) {
                        // Initializing failed, delete the disk
                        delete disk;
                    } else
                        m_diskList.pushBack(disk);
                    break;
                }
            }
        }
    }
} // namespace Quark::System::Hal