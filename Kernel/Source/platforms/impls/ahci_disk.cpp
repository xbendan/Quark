#include <drivers/storage/ahci/device.h>
#include <quark/memory/address_range.h>
#include <quark/memory/address_space.h>

namespace Quark::System::Hal {
    using Quark::System::Memory::AddressRange;

    SATADiskDevice::SATADiskDevice(int                   port,
                                   AHCI::HBAPortRegs*    portRegs,
                                   AHCIControllerDevice* controller)
        : Io::DiskDevice("Unidentified SATA Hard Disk Drive")
        , m_portRegs(portRegs)
        , m_memRegs(controller->memRegs())
    {
        stopCommand();

        portRegs->_clb  = (controller->clbPhys() & 0xFFFFFFFF) + (port << 10);
        portRegs->_clbu = (controller->clbPhys() >> 32);

        portRegs->_fb  = (controller->fbPhys() & 0xFFFFFFFF) + (port << 8);
        portRegs->_fbu = (controller->fbPhys() >> 32);

        m_commandList = reinterpret_cast<AHCI::_HBACommandHeader*>(
            Memory::copyAsIOAddress(portRegs->_clb));

        for (int i = 0; i < 32; i++) {
            m_commandList[i]._prdtl = 8;

            u64 phys = controller->ctbaPhys() + (port << 13) + (i << 8);
            m_commandList[i]._ctba  = (phys & 0xFFFFFFFF);
            m_commandList[i]._ctbau = (phys >> 32);

            AddressRange((u64)(m_commandList[i]._ctba), 256).clear();

            m_commandTable[i] = reinterpret_cast<AHCI::HBACommandTable*>(
                Memory::copyAsIOAddress(phys));
        }

        startCommand();
    }

    // SATADiskDevice::~SATADiskDevice() {}

    i64 SATADiskDevice::read(u64 offset, u64 size, void* buffer)
    {
        return access(offset / 512, size / 512, (u64)buffer, false);
    }

    i64 SATADiskDevice::write(u64 offset, u64 size, void* buffer)
    {
        return access(offset / 512, size / 512, (u64)buffer, true);
    }

    void SATADiskDevice::startCommand()
    {
        while (m_portRegs->_cmd & HBA_PxCMD_CR)
            ;
        m_portRegs->_cmd |= (HBA_PxCMD_FRE | HBA_PxCMD_ST);
    }

    void SATADiskDevice::stopCommand()
    {
        m_portRegs->_cmd &= ~(HBA_PxCMD_FRE | HBA_PxCMD_ST);
        while ((m_portRegs->_cmd & HBA_PxCMD_FR) ||
               (m_portRegs->_cmd & HBA_PxCMD_CR))
            ;
    }

    int SATADiskDevice::access(u64 lba, u32 count, u64 physBuf, int write)
    {
        m_portRegs->_ie = 0xffffffff;
        m_portRegs->_is = 0;
        int spin = 0, slot = findSlot();

        if (slot == -1)
            return false;

        m_portRegs->_serr = 0;
        m_portRegs->_tfd  = 0;

        AHCI::HBACommandHeader* header = &m_commandList[slot];
        header->_cfl   = sizeof(AHCI::RegHostToDevice) / sizeof(u32);
        header->_a     = 0;
        header->_w     = write;
        header->_c     = 0;
        header->_p     = 0;
        // header->_prdtl = (u16)((count - 1) >> 4) + 1;
        header->_prdbc = 0;
        header->_pmp   = 0;

        AHCI::HBACommandTable* table = m_commandTable[slot];
        AddressRange((u64)table, sizeof(AHCI::HBACommandTable)).clear();
        table->_prdtEntry[0]._dba  = physBuf & 0xFFFFFFFF;
        table->_prdtEntry[0]._dbau = (physBuf >> 32) & 0xFFFFFFFF;
        table->_prdtEntry[0]._dbc  = (count << 9) - 1;
        table->_prdtEntry[0]._i    = 1;

        AHCI::RegHostToDevice* h2d = (AHCI::RegHostToDevice*)table->_cfis;
        AddressRange((u64)h2d, sizeof(AHCI::RegHostToDevice)).clear();
        h2d->_fisType        = AHCI::FisRegs::TYPE_REG_H2D;
        h2d->_mode           = 1;
        h2d->_portMultiplier = 0;
        h2d->_command   = write ? ATA_CMD_WRITE_DMA_EX : ATA_CMD_READ_DMA_EX;
        h2d->_lba0      = (u8)(lba & 0xFF);
        h2d->_lba1      = (u8)((lba >> 8) & 0xFF);
        h2d->_lba2      = (u8)((lba >> 16) & 0xFF);
        h2d->_lba3      = (u8)((lba >> 24) & 0xFF);
        h2d->_lba4      = (u8)((lba >> 32) & 0xFF);
        h2d->_lba5      = (u8)((lba >> 40) & 0xFF);
        h2d->_device    = 1 << 6;
        h2d->_countLow  = count & 0xFF;
        h2d->_countHigh = (count >> 8) & 0xFF;
        h2d->_ctl       = 0x8;

        while ((m_portRegs->_tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) &&
               spin < 100'0000)
            spin++;
        if (spin == 100'0000) {
            return false;
        }

        m_portRegs->_ci = 1 << slot;
        while (true) {
            if (!(m_portRegs->_ci & (1 << slot)))
                break;

            if (m_portRegs->_is & HBA_PxIS_TFES) {
                //
                return false;
            }
        }

        return !(m_portRegs->_is & HBA_PxIS_TFES);
    }

    int SATADiskDevice::findSlot()
    {
        u32 slots = (m_portRegs->_sact | m_portRegs->_ci);
        for (int i = 0; i < 32; i++) {
            if (!(slots & 1)) {
                return i;
            }
            slots >>= 1;
        }
        return -1;
    }
} // namespace Quark::System::Hal