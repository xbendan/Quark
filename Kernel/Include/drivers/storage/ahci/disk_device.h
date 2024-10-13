#pragma once

#include <drivers/storage/ahci/spec.h>
#include <quark/dev/disk_device.h>

namespace AHCI {
    using namespace Quark::System::Io;

    class AHCIControllerDevice;

    class SATADiskDevice : public DiskDevice
    {
    public:
        SATADiskDevice(int                   port,
                       AHCI::HBAPortRegs*    portRegs,
                       AHCIControllerDevice* controller);
        virtual ~SATADiskDevice() override;

        i64 Read(u64 offset, u64 size, void* buffer) override;
        i64 Write(u64 offset, u64 size, void* buffer) override;

        void StartCommand();
        void StopCommand();

        int Status() { return m_status; }

        int Access(u64 lba, u32 count, u64 physBuffer, int write);

        int FindSlot();

    private:
        AHCI::HBAPortRegs*      m_portRegs;
        AHCI::HBAMemRegs*       m_memRegs;
        AHCI::HBACommandHeader* m_commandList;
        AHCI::HBACommandTable*  m_commandTable[8];
        AHCI::HBAFrameInfo*     m_fis;
        int                     m_status;

        u64 m_physBuffer[8];
    };
}