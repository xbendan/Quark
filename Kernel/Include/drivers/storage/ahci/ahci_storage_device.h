#pragma once

#include <drivers/storage/ahci/spec.h>
#include <quark/dev/storage/storage_device.h>

namespace AHCI {
    using namespace Quark::System;

    class AHCIStorageControllerDevice;

    class SATAStorageDevice final : public Io::StorageDevice
    {
    public:
        SATAStorageDevice(int                          port,
                          AHCI::HBAPortRegs*           portRegs,
                          AHCIStorageControllerDevice* controller,
                          u64                          driveSize);
        ~SATAStorageDevice();

        virtual Res<usize> Read(u64, Bytes) override;
        virtual Res<usize> Write(u64, Bytes) override;

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