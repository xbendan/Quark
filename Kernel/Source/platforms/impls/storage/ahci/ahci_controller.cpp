#include <drivers/storage/ahci/controller_device.h>
#include <quark/memory/address_space.h>
#include <quark/memory/page_alloc.h>

namespace AHCI {
    using namespace Quark::System::Memory;

    AHCIStorageControllerDevice::AHCIStorageControllerDevice(PCIInfo& info)
        : PCIDevice(info, "AHCI Controller Device", Type::StorageController)
        , m_clbPhys(AllocatePhysMemory4K(8).unwrap())
        , m_fbPhys(AllocatePhysMemory4K(2).unwrap())
        , m_ctbaPhys(AllocatePhysMemory4K(64).unwrap())
        , m_addrPhys(GetBaseAddrRegs(5))
    {
        EnableBusMastering();
        EnableInterrupts();
        EnableMemorySpace();

        m_addrVirt = CopyAsIOAddress(m_addrPhys);
        m_memRegs  = reinterpret_cast<AHCI::HBAMemRegs*>(m_addrVirt);

        m_memRegs->GlobalHostControl &= ~AHCI_GHC_IE;
        m_memRegs->InterruptStatus = 0xFFFFFFFF;

        for (int i = 0; i < 32; i++) {
            if (!(m_memRegs->PortsImplemented & (1 << i))) {
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
                    auto* disk = new SATAStorageDevice(i, portRegs, this, 0);

                    if (disk->Status() != 1) {
                        // Initializing failed, delete the disk
                        delete disk;
                    } else
                        m_diskList.PushBack(disk);
                    break;
                }
            }
        }
    }
} // namespace Quark::System::Hal