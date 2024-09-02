#include <mixins/std/c++types.h>
#include <mixins/utils/uuid.h>

namespace SMBIOS {
    struct Type2EntryPoint
    {
        char _signature[4];
        u8   _checksum;
        u8   _length;
        u8   _majorVer;
        u8   _minorVer;
        u16  _maxSize;
        u8   _revision;
        char _formatArea[5];
        char _signature2[5];
        u8   _checksum2;
        u16  _tableLength;
        u32  _tableAddress;
        u16  _structNums;
        u8   _bcdRevision;
    };

    struct Type3EntryPoint
    {
        char _signature[5];
        u8   _checksum;
        u8   _length;
        u8   _majorVer;
        u8   _minorVer;
        u8   _docrev;
        u8   _revision;
        u8   _reserved0;
        u32  _maxSize;
        u64  _tableAddress;
    };

    struct TableHeader
    {
        u8  _type;
        u8  _length;
        u16 _handle;
    };

#define BIOS_CHARS_NOT_SUPPORTED (1 << 3)
#define BIOS_CHARS_ISA (1 << 4)
#define BIOS_CHARS_MCA (1 << 5)
#define BIOS_CHARS_EISA (1 << 6)
#define BIOS_CHARS_PCI (1 << 7)
#define BIOS_CHARS_PCMCIA (1 << 8)
#define BIOS_CHARS_PNP (1 << 9) /* Plug and Play */
#define BIOS_CHARS_APM (1 << 10)
#define BIOS_CHARS_IS_UPGRADEABLE (1 << 11)
#define BIOS_CHARS_SHADOWING (1 << 12)
#define BIOS_CHARS_VL_VESA (1 << 13)
#define BIOS_CHARS_ESCD (1 << 14)
#define BIOS_CHARS_BOOT_FROM_CD (1 << 15)
#define BIOS_CHARS_SELECTABLE_BOOT (1 << 16)
#define BIOS_CHARS_ROM_SOCKETED (1 << 17)
#define BIOS_CHARS_BOOT_FROM_PCMCIA (1 << 18)
#define BIOS_CHARS_EDD_SPEC (1 << 19)
#define BIOS_CHARS_JAPFLOPPY_NEC9800 (1 << 20)
#define BIOS_CHARS_JAPFLOPPY_TOSHIBA (1 << 21)
#define BIOS_CHARS_FLOPPY_S525_360 (1 << 22)
#define BIOS_CHARS_FLOPPY_S525_12 (1 << 23)
#define BIOS_CHARS_FLOPPY_S35_720 (1 << 24)
#define BIOS_CHARS_FLOPPY_S35_288 (1 << 25)
#define BIOS_CHARS_PRINT_SCREEN (1 << 26)
#define BIOS_CHARS_8042KEYBOARD (1 << 27)
#define BIOS_CHARS_SERIAL (1 << 28)
#define BIOS_CHARS_PRINTER (1 << 29)
#define BIOS_CHARS_CGA_MONOVIDEO (1 << 30)
#define BIOS_CHARS_NEC_PC98 (1 << 31)

#define BIOS_CHARSEXT_ACPI 1
#define BIOS_CHARSEXT_USB_LEGACY (1 << 1)
#define BIOS_CHARSEXT_AGP (1 << 2)
#define BIOS_CHARSEXT_I2O_BOOT (1 << 3)
#define BIOS_CHARSEXT_LS120_BOOT (1 << 4)
#define BIOS_CHARSEXT_ATAPI_ZIP_BOOT (1 << 5)
#define BIOS_CHARSEXT_1394_BOOT (1 << 6)
#define BIOS_CHARSEXT_SMART_BATTERY (1 << 7)

#define BIOS_CHARSEXTT_BOOT_SPECS 1
/*
 * Function key-initiated network service boot is supported. When function
 * key-uninitiated network service boot is not supported, a network adapter
 * option ROM may choose to offer this functionality on its own, thus offering
 * this capability to legacy systems. When the function is supported, the
 * network adapter option ROM shall not offer this capability.
 */
#define BIOS_CHARSEXTT_FUNC_KEYINIT_NETWORK (1 << 1)
#define BIOS_CHARSEXTT_TARGETED_CONTENT_DIST (1 << 2)
#define BIOS_CHARSEXTT_UEFI_SPECS (1 << 3)
#define BIOS_CHARSEXTT_VIRT_MACHINE (1 << 4)
#define BIOS_CHARSEXTT_MANU_SUPPORTED (1 << 5)
#define BIOS_CHARSEXTT_MANU_ENABLED (1 << 6)

    /**
     * @brief BIOS Information, Type 0
     *
     */
    struct BIOSInfoTable : public TableHeader
    {
        u8  _vendor;
        u8  _version;
        u16 _startingAddrSeg;
        u8  _releaseDate;
        u8  _romSize;
        u64 _chars;
        // Spec. Ver >= 2.4
        u16 _charsExtBytes;
        u8  _majorRel;
        u8  _minorRel;
        u8  _ecfMajorRel;
        u8  _ecfMinorRel;
    };

    /**
     * @brief System Information, Type 1
     *
     * UUID byte order and RFC4122 field names
     * ----|-----------------------|------------------------------------------
     * 0x0  time_low                (u32)  Low field of the timestamp
     *
     * 0x4  time_mid                (u16)  Middle field of the timestamp
     *
     * 0x6  time_hi_and_version     (u16)  High field of the timestamp
     *                                          multiplexed with the version
     *                                          number
     *
     * 0x8  clock_seq_hi_and_reserved (u8) High field of the clock
     *                                          sequence multiplexed with the
     *                                          variant
     *
     * 0x9  clock_seq_low           (u8)   Low field of the clock sequence
     *
     * 0xa  node                    (u8[6]) Spatially unique node
     *                                           identifier
     * ----|-----------------------|------------------------------------------
     *
     *
     * Wakeup Type
     * ----|-----------------------|------------------------------------------
     * 0x0  Reserved
     * 0x1  Other
     * 0x2  Unknown
     * 0x3  APM Timer
     * 0x4  Modem Ring
     * 0x5  LAN Remote
     * 0x6  Power Switch
     * 0x7  PCI PME#
     * 0x8  AC Power Restored
     * ----|-----------------------|------------------------------------------
     */
    struct SystemInfoTable : public TableHeader
    {
        char _manufacturer;
        char _productName;
        char _version;
        char _serialNumber;
        UUID _uuid;
        u8   _wakeupType;
        char _skuNumber;
        char _family;
    };

    /**
     * @brief Baseboard (or Module) Information, Type 2
     *
     * Board Type
     * ----|-----------------------|------------------------------------------
     * 0x1  Unknown
     * 0x2  Other
     * 0x3  Server Blade
     * 0x4  Connect Switch
     * 0x5  System Management Module
     * 0x6  Processor Module
     * 0x7  I/O Module
     * 0x8  Memory Module
     * 0x9  Daughter Board
     * 0xa  Motherboard
     * 0xb  Processor/Memory Module
     * 0xc  Processor/IO Module
     * 0xd  Interconnect Board
     * ----|-----------------------|------------------------------------------
     *
     *
     * Feature Flags
     * ----|------------------------------------------------------------------
     * 0x0  Set to 1 if the board is a hosting board
     * 0x1  Set to 1 if the board requires at least one daughter board or
     *      auxiliary card to be fully functional
     * 0x2  Set to 1 if the board is removable
     * 0x3  Set to 1 if the board is replaceable
     * 0x4  Set to 1 if the board is hot swappable
     *
     * 0x5 ~ 0x7  Reserved
     * ----|------------------------------------------------------------------
     */
    struct BaseboardInfoTable : public TableHeader
    {
        char _manufacturer;
        char _productName;
        char _version;
        char _serialNumber;
        char _assetTag;
        u8   _featureFlags;
        char _locationInChassis;
        u16  _chassisHandle;
        u8   _boardType;
        u8   _numContainedObjectHandles;
        u16  _containedObjectHandles[1];
    };

    /**
     * @brief System Enclosure or Chassis, Type 3
     *
     * Type
     * -----|-----------------------|------------------------------------------
     * 0x01  Other
     * 0x02  Unknown
     * 0x03  Desktop
     * 0x04  Low Profile Desktop
     * 0x05  Pizza Box
     * 0x06  Mini Tower
     * 0x07  Tower
     * 0x08  Portable
     * 0x09  Laptop
     * 0x0a  Notebook
     * 0x0b  Hand Held
     * 0x0c  Docking Station
     * 0x0d  All in One
     * 0x0e  Sub Notebook
     * 0x0f  Space-Saving
     * 0x10  Lunch Box
     * 0x11  Main Server Chassis
     * 0x12  Expansion Chassis
     * 0x13  SubChassis
     * 0x14  Bus Expansion Chassis
     * 0x15  Peripheral Chassis
     * 0x16  RAID Chassis
     * 0x17  Rack Mount Chassis
     * 0x18  Sealed-Case PC
     * 0x19  Multi-system            When this value is specified, by an SMBIOS
     *                               implementation, the physical chassis
     *                               associated with this structure supports
     *                               multiple, independently reporting physical
     *                               systems--regardless of the chassis' current
     *                               configuration.
     * 0x1a  CompactPCI
     * 0x1b  AdvancedTCA
     * 0x1c  Blade
     * 0x1d  Blade Enclosure
     * 0x1e  Tablet
     * 0x1f  Convertible
     * 0x20  Detachable
     * 0x21  IoT Gateway
     * 0x22  Embedded PC
     * 0x23  Mini PC
     * 0x24  Stick PC
     * -----|-----------------------|------------------------------------------
     *
     * States
     * -----|-----------------------|------------------------------------------
     * 0x01  Other
     * 0x02  Unknown
     * 0x03  Safe
     * 0x04  Warning
     * 0x05  Critical
     * 0x06  Non-recoverable
     * -----|-----------------------|------------------------------------------
     *
     * Security Status
     * -----|-----------------------|------------------------------------------
     * 0x01  Other
     * 0x02  Unknown
     * 0x03  None
     * 0x04  External Interface Locked Out
     * 0x05  External Interface Enabled
     * -----|-----------------------|------------------------------------------
     */
    struct SystemEnclosureTable : public TableHeader
    {
        char _manufacturer;
        u8   _type;
        char _version;
        char _serialNumber;
        char _assetTag;
        u8   _bootupState;
        u8   _powerSupplyState;
        u8   _thermalState;
        u8   _securityStatus;
        u32  _oemDefined;
        u8   _height;
        u8   _numPowerCords;
        u8   _containedElementCount;
        u8   _containedElementRecordLength;
        u8   _containedElements[1];
    };

    /**
     * @brief Processor Information, Type 4
     *
     */
    struct ProcessorInfoTable : public TableHeader
    {
        u8  _socketDesignation;
        u8  _processorType;
        u8  _processorFamily;
        u8  _processorManufacturer;
        u64 _processorId;
        u8  _processorVersion;
        u8  _voltage;
        u16 _externalClock;
        u16 _maxSpeed;
        u16 _currentSpeed;
        u8  _status;
        u8  _processorUpgrade;
        u16 _l1CacheHandle;
        u16 _l2CacheHandle;
        u16 _l3CacheHandle;
        u8  _serialNumber;
        u8  _assetTag;
        u8  _partNumber;
        u8  _coreCount;
        u8  _coreEnabled;
        u8  _threadCount;
        u16 _processorCharacteristics;
        u16 _processorFamily2;
        u16 _coreCount2;
        u16 _coreEnabled2;
        u16 _threadCount2;
    };

    /**
     * @brief Cache Information, Type 7
     *
     */
    struct CacheInfoTable : public TableHeader
    {};

    /**
     * @brief Port Connector Information, Type 8
     *
     */
    struct SlotsInfoTable : public TableHeader
    {};

    /**
     * @brief Physical Memory Array, Type 16
     *
     */
    struct PhysMemArrayTable : public TableHeader
    {};

    /**
     * @brief Memory Device, Type 17
     *
     */
    struct MemoryDeviceTable : public TableHeader
    {};

    /**
     * @brief Memory Array Mapped Address, Type 19
     *
     */
    struct MemArrayMappedAddressTable : public TableHeader
    {};

    /**
     * @brief System Boot Information, Type 32
     *
     */
    struct BootInfoTable : public TableHeader
    {};
} // namespace SMBIOS