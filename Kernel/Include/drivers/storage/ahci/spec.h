#pragma once

#include <mixins/std/c++types.h>

#define AHCI_GHC_HR (1 << 0) // HBA Reset
#define AHCI_GHC_IE (1 << 1) // Interrupt enable
#define AHCI_GHC_ENABLE (1 << 31)

#define AHCI_CAP_S64A (1 << 31) // 64-bit addressing
#define AHCI_CAP_NCQ (1 << 30)  // Support for Native Command Queueing?
#define AHCI_CAP_SSS (1 << 27)  // Supports staggered Spin-up?
#define AHCI_CAP_FBSS (1 << 16) // FIS-based switching supported?
#define AHCI_CAP_SSC (1 << 14)  // Slumber state capable?
#define AHCI_CAP_PSC (1 << 13)  // Partial state capable
#define AHCI_CAP_SALP (1 << 26) // Supports aggressive link power management

#define AHCI_CAP2_NVMHCI (1 << 1) // NVMHCI Present
#define AHCI_CAP2_BOHC (1 << 0)   // BIOS/OS Handoff

#define AHCI_BOHC_BIOS_BUSY (1 << 4)    // BIOS Busy
#define AHCI_BOHC_OS_OWNERSHIP (1 << 3) // OS Ownership Change

#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_WRITE_DMA_EX 0x35
#define ATA_CMD_IDENTIFY 0xec
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08

#define SATA_SIG_SATA 0x00000101  // SATA drive
#define SATA_SIG_ATAPI 0xEB140101 // SATAPI drive
#define SATA_SIG_SEMB 0xC33C0101  // Enclosure management bridge
#define SATA_SIG_PM 0x96690101    // Port multiplier

#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_SUD 0x0002
#define HBA_PxCMD_POD 0x0004
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_FR 0x4000
#define HBA_PxCMD_CR 0x8000
#define HBA_PxCMD_ASP 0x4000000 // Aggressive Slumber/Partial
#define HBA_PxCMD_ICC (0xf << 28)
#define HBA_PxCMD_ICC_ACTIVE (1 << 28)

#define HBA_PxIS_TFES (1 << 30)

#define HBA_PORT_IPM_ACTIVE 1

#define HBA_PxSSTS_DET 0xfULL
#define HBA_PxSSTS_DET_INIT 1
#define HBA_PxSSTS_DET_PRESENT 3

namespace AHCI {
    enum FisRegs : u8
    {
        TYPE_REG_H2D   = 0x27,
        TYPE_REG_D2H   = 0x34,
        TYPE_DMA_ACT   = 0x39,
        TYPE_DMA_SETUP = 0x41,
        TYPE_DATA      = 0x46,
        TYPE_BIST      = 0x58,
        TYPE_PIO_SETUP = 0x5F,
        TYPE_DEV_BITS  = 0xA1,
    };

    struct RegHostToDevice
    {
        u8 _fisType;

        u8 _portMultiplier : 4;
        u8 _reserved0 : 3;
        u8 _mode : 1;

        u8 _command;
        u8 _featLow;

        union
        {
            dword _dw0;

            struct
            {
                u8 _lba0;
                u8 _lba1;
                u8 _lba2;
                u8 _device;
            };
        };

        union
        {
            dword _dw1;

            struct
            {
                u8 _lba3;
                u8 _lba4;
                u8 _lba5;
                u8 _featHigh;
            };
        };

        union
        {
            dword _dw2;

            struct
            {
                u8 _countLow;
                u8 _countHigh;
                u8 _icc;
                u8 _ctl;
            };
        };

        union
        {
            dword _dw3;

            struct
            {
                u8 _reserved1[4];
            };
        };
    } __attribute__((packed));

    struct RegDeviceToHost
    {
        u8 _fisType;

        u8 _portMultiplier : 4;
        u8 _reserved0 : 2;
        u8 _interrupt : 1;
        u8 _reserved1 : 1;

        u8 _statusRegister;
        u8 _errorRegister;

        union
        {
            dword _dw0;

            struct
            {
                u8 _lba0;
                u8 _lba1;
                u8 _lba2;
                u8 _device;
            };
        };

        union
        {
            dword _dw1;

            struct
            {
                u8 _lba3;
                u8 _lba4;
                u8 _lba5;
                u8 _reserved2;
            };
        };

        union
        {
            dword _dw2;

            struct
            {
                u8 _countLow;
                u8 _countHigh;
                u8 _reserved3[2];
            };
        };

        union
        {
            dword _dw3;

            struct
            {
                u8 _reserved4[4];
            };
        };
    } __attribute__((packed));

    struct Data
    {
        u8 _fisType;

        u8 _portMultiplier : 4;
        u8 _reserved0 : 4;

        u8 _reserved1[2];

        u32 _data[1];
    } __attribute__((packed));

    struct PIOSetup
    {
        u8 _fisType;

        u8 _portMultiplier : 4;
        u8 _reserved0 : 1;
        u8 _interrupt : 1;
        u8 _reserved1 : 1;
        u8 _status : 1;

        u8 _error;

        union
        {
            dword _dw0;

            struct
            {
                u8 _lba0;
                u8 _lba1;
                u8 _lba2;
                u8 _device;
            };
        };

        union
        {
            dword _dw1;

            struct
            {
                u8 _lba3;
                u8 _lba4;
                u8 _lba5;
                u8 _reserved2;
            };
        };

        union
        {
            dword _dw2;

            struct
            {
                u8 _countLow;
                u8 _countHigh;
                u8 _reserved3;
                u8 _e_status;
            };
        };

        union
        {
            dword _dw3;

            struct
            {
                u16 _transferCount;
                u8  _reserved4[2];
            };
        };
    } __attribute__((packed));

    struct DMASetup
    {
        u8 _fisType;

        u8 _portMultiplier : 4;
        u8 _reserved0 : 1;
        u8 _interrupt : 1;
        u8 _autoActivate : 1;
        u8 _reserved1 : 1;

        u8 _reserved2[2];

        u64 _dmaBufferId;

        u32 _reserved3;

        u32 _dmaBufferOffset;

        u32 _transferCount;

        u32 _reserved4;
    } __attribute__((packed));

    struct _HBAPortRegs
    {
        u32 _clb;
        u32 _clbu;
        u32 _fb;
        u32 _fbu;
        u32 _is;
        u32 _ie;
        u32 _cmd;
        u32 _reserved0;
        u32 _tfd;
        u32 _sig;
        u32 _ssts;
        u32 _sctl;
        u32 _serr;
        u32 _sact;
        u32 _ci;
        u32 _sntf;
        u32 _fbs;
        u32 _reserved1[11];
        u32 _vendor[4];
    } __attribute__((packed));

    using HBAPortRegs = volatile _HBAPortRegs;

    struct _HBAMemRegs
    {
        u32          _hostCapability;
        u32          _ghc;
        u32          _interruptStatus;
        u32          _portsImplemented;
        u32          _version;
        u32          _ccc_ctl;
        u32          _ccc_pts;
        u32          _em_loc;
        u32          _em_ctl;
        u32          _hostCapabilityExt;
        u32          _bohc;
        u8           _reserved0[0x74];
        u8           _vendor[0x60];
        _HBAPortRegs _ports[0x20];
    } __attribute__((packed));

    using HBAMemRegs = volatile _HBAMemRegs;

    struct _HBAFrameInfo
    {
        DMASetup _dmaSetup;
        u8       _reserved0[4];

        PIOSetup _pioSetup;
        u8       _reserved1[12];

        RegDeviceToHost _d2hRegister;
        u8              _reserved2[4];

        u8 devBits[8];

        u8 ufis[64];

        u8 reserved3[0x60];
    } __attribute__((packed));

    using HBAFrameInfo = volatile _HBAFrameInfo;

    struct _HBACommandHeader
    {
        u8 _cfl : 5;
        u8 _a : 1;
        u8 _w : 1;
        u8 _p : 1;

        u8 _r : 1;
        u8 _b : 1;
        u8 _c : 1;
        u8 _reserved0 : 1;
        u8 _pmp : 4;

        u16 _prdtl;

        volatile u32 _prdbc;

        u32 _ctba;

        u32 _ctbau;

        u32 _reserved1[4];
    } __attribute__((packed));

    using HBACommandHeader = volatile _HBACommandHeader;

    struct HbaPrdtEntry
    {
        u32 _dba;

        u32 _dbau;

        u32 _reserved0;

        u32 _dbc : 22;
        u32 _reserved1 : 9;
        u32 _i : 1;
    } __attribute__((packed));

    struct _HBACommandTable
    {
        u8 _cfis[64];

        u8 _acmd[16];

        u8 _reserved0[48];

        HbaPrdtEntry _prdtEntry[1];
    } __attribute__((packed));

    using HBACommandTable = volatile _HBACommandTable;

} // namespace AHCI