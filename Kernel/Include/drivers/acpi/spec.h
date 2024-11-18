#pragma once

#include <mixins/std/c++types.h>

namespace ACPI {
    struct RootSystemDescPointer
    {
        char _signature[8];
        u8   _checksum;
        char _oemId[6];
        u8   _revision;
        u32  _rsdtAddress;
    } __attribute__((packed));

    struct ExtendedSystemDescPointer : public RootSystemDescPointer
    {
        u32 _length;
        u64 _xsdtAddress;
        u8  _checksumEx;
        u8  _reserved[3];
    } __attribute__((packed));

    struct TableHeader
    {
        char _signature[4];
        u32  _length;
        u8   _revision;
        u8   _checksum;
        char _oemId[6];
        char _oemTableId[8];
        u32  _oemRevision;
        u32  _creatorId;
        u32  _creatorRevision;
    } __attribute__((packed));

    struct RootSystemDescTable : public TableHeader
    {
        u32 _pointers[];
    } __attribute__((packed));

    struct ExtendedSystemDescTable : public TableHeader
    {
        u64 _pointers[];
    } __attribute__((packed));

    struct AddressPack
    {
        /**
         * @brief address space indicates where to read data
         * 0: System Memory
         * 1: System I/O
         * 2: PCI Configuration Space
         * 3: Embedded Controller
         * 4: @deprecated System Management Bus
         * 5: System CMOS
         * 6: PCI Device BAR Target
         * 7: @deprecated Intelligent Platform Management Infrastructure
         * 8: @deprecated General Purpose I/O
         * 9: @deprecated Generic Serial Bus
         * 0x0a: @deprecated Platform Communication Channel
         * 0x0b-0x7f: reserved
         * 0x80-0xff: OEM defined
         */
        u8  _addressSpace;
        u8  _bitWidth;
        u8  _bitOffset;
        u8  _accessSize;
        u64 _address;
    } __attribute__((packed));

    struct MultiApicDescTable /* Multiple APIC Description Table */
        : public TableHeader
    {
        u32 _address;
        u32 _flags;

        struct Entry
        {
            u8 _type;
            u8 _length;
        } __attribute__((packed)) _entries[];

        struct LocalApic : public Entry
        {
            u8  _processorId;
            u8  _apicId;
            u32 _flags;
        } __attribute__((packed));

        struct IoApic : public Entry
        {
            u8  _apicId;
            u8  __reserved__;
            u32 _address;
            u32 _gSiB;
        } __attribute__((packed));

        struct InterruptServiceOverride : public Entry
        {
            u8  _busSource;
            u8  _irqSource;
            u32 _gSi;
            u16 _flags;
        } __attribute__((packed));

        struct NonMaskableInterrupt : public Entry
        {
            u8  _processorId;
            u16 _flags;
            u8  _lInt;
        } __attribute__((packed));

        struct Localx2Apic : public Entry
        {
            u16 __reserved__;
            u32 _x2apicId;
            u32 _flags;
            u32 _uid;
        } __attribute__((packed));

        struct Nmix2Apic : public Entry
        {
            u16 _flags;
            u32 _uid;
            u8  _lInt;
            u8  __reserved__[3];
        } __attribute__((packed));
    } __attribute__((packed));

    using MADT = MultiApicDescTable;

    struct HighPrecisionEventTable : public TableHeader
    {
        u8          _hwrevId;
        u8          _info;
        u16         _pciVendorId;
        AddressPack _address;
        u8          _hpetNumber;
        u16         _minTick;
        u8          _pageProtection;
    } __attribute__((packed));

    struct PCIExpressSpecTable : public TableHeader
    {
        u64 __reserved__;

        struct AddressPacket
        {
            u64 _base;
            u16 _sgn;
            u8  _busStart;
            u8  _busEnd;
            u32 __reserved__;
        } __attribute__((packed)) _baseAddresses[];
    } __attribute__((packed));

    struct FixedAcpiDescTable /* Fixed ACPI Description Table */
        : public TableHeader
    {
        u32 _fwctrl;
        u32 _dsdt;

        u8 __reserved__0;

        u8  _ppmp;
        u16 _sciInt;
        u32 _smiCmd;
        u8  _enable;
        u8  _disable;
        u8  _s4biosReq;
        u8  _pStateCtrl;
        u32 _pm1aEventBlock;
        u32 _pm1bEventBlock;
        u32 _pm1aCtrlBlock;
        u32 _pm1bCtrlBlock;
        u32 _pm2CtrlBlock;
        u32 _pmtTimerBlock;
        u32 _gpe0Block;
        u32 _gpe1Block;
        u8  _pm1EventLength;
        u8  _pm1CtrlLength;
        u8  _pm2CtrlLength;
        u8  _pmtTimerLength;
        u8  _gpe0Length;
        u8  _gpe1Length;
        u8  _gpe1Base;
        u8  _cStateCtrl;
        u16 _worstC2Latency;
        u16 _worstC3Latency;
        u16 _flushSize;
        u16 _flushStride;
        u8  _dutyOffset;
        u8  _dutyWidth;
        u8  _dayAlarm;
        u8  _monthAlarm;
        u8  _century;

        u16 _bootFlagIAPC;

        u8 __reserved__1;

//         struct
//         {
//             /*
//                 If set, signifies that the WBINVD instruction
//                 correctly flushes
// the processor caches, maintains memory coherency, and upon completion
// of the instruction, all caches for the current processor contain no
// cached data other than what OSPM references and allows to be cached.
// If this flag is not set, the ACPI OS is responsible for disabling all
// ACPI features that need this function. This field is maintained for
// ACPI 1.0 processor compatibility on existing systems. Processors in
// new ACPIcompatible systems are required to support this function and
// indicate this to OSPM by setting this field.
//                 如果设置，则表示WBINVD指令正确地刷新处理器缓存，保持内存一致性，并且在指令完成后，当前处理器的所有缓存都不包含除了OSPM引用和允许缓存的数据之外的缓存数据。
// 如果不设置该标志，则ACPI操作系统负责禁用所有需要该功能的ACPI特性。
// 该字段是为了在现有系统上兼容ACPI 1.0处理器而维护的。
// 新的acpiccompatible系统中的处理器需要支持此功能，并通过设置此字段向OSPM表明此功能。
//             */
//             u32 WbInvd : 1;
//             u32 WbInvdFlush : 1;
//             u32 ProcC1 : 1;
//             u32 P_LVL2Up : 1;
//         } _flags;
#define FADT_FLAGS_WBINVD (1 << 0)
#define FADT_FLAGS_WBINVD_FLUSH (1 << 1)
#define FADT_FLAGS_PROC_C1 (1 << 2)
#define FADT_FLAGS_P_LVL2UP (1 << 3)
#define FADT_FLAGS_PWR_BUTTON (1 << 4)
#define FADT_FLAGS_SLP_BUTTON (1 << 5)
#define FADT_FLAGS_FIX_RTC (1 << 6)
#define FADT_FLAGS_RTC_S4 (1 << 7)
#define FADT_FLAGS_TMR_VAL_EXT (1 << 8)
#define FADT_FLAGS_DCK_CAP (1 << 9)
#define FADT_FLAGS_RESET_REG_SUP (1 << 10)
#define FADT_FLAGS_SEALED_CASE (1 << 11)
#define FADT_FLAGS_HEADLESS (1 << 12)
#define FADT_FLAGS_CPU_SW_SLP (1 << 13)
#define FADT_FLAGS_PCI_EXP_WAK (1 << 14)
#define FADT_FLAGS_USE_PLATFORM_CLOCK (1 << 15)
#define FADT_FLAGS_S4_RTC_STS_VALID (1 << 16)
#define FADT_FLAGS_REMOTE_POWER_ON_CAP (1 << 17)
#define FADT_FLAGS_APIC_CLUSTER (1 << 18)
#define FADT_FLAGS_APIC_PHYSICAL (1 << 19)
#define FADT_FLAGS_HW_REDUCED_ACPI (1 << 20)
#define FADT_FLAGS_LOW_POWER_S0 (1 << 21)
        u32 _flags;

        AddressPack _resetRegistry;
        u8          _resetValue;
        u16         _bootFlagARM;
        u8          _minor;

        u64         x_fwctrl;
        u64         x_dsdt;
        AddressPack x_pm1aEventBlock;
        AddressPack x_pm1bEventBlock;
        AddressPack x_pm1aCtrlBlock;
        AddressPack x_pm1bCtrlBlock;
        AddressPack x_pm2CtrlBlock;
        AddressPack x_pmtTimerBlock;
        AddressPack x_gpe0Block;
        AddressPack x_gpe1Block;
        AddressPack x_sleepCtrl;
        AddressPack x_sleepStatus;
        u64         x_hypervisorVendorId;
    } __attribute__((packed));

    struct FACS /* Firmware ACPI Control Structure */
    {
    } __attribute__((packed));

    struct SmartBatteryDescTable /* Smart Battery Description Table */
        : public TableHeader
    {
        u32 _warningEnergyLevel;
        u32 _lowEnergyLevel;
        u32 _criticalEnergyLevel;
    } __attribute__((packed));

    struct EmbedCtrlBootResTable /* Embedded Controller Boot Resources Table */
        : public TableHeader
    {
        AddressPack _ecCtrl;
        AddressPack _ecData;
        u32         _uid;
        u8          _gpeBit;
        u8          _ecId[];
    } __attribute__((packed));

    struct DiffSystemDescTable /* Differentiated System Description Table */
        : public TableHeader
    {
        u8 _aml[];
    };

    struct SecdSystemDescTable /* Secondary System Description Table */
        : public TableHeader
    {
        u8 _aml[];
    };
} // namespace ACPI