#pragma once

#include <mixins/std/c++types.h>

#define PCI_BIST_CAPABLE (1 << 7)
#define PCI_BIST_START (1 << 6)
#define PCI_BIST_COMPLETION_CODE (0xF)

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_CMD_INTERRUPT_DISABLE (1 << 10)
#define PCI_CMD_FAST_BTB_ENABLE (1 << 9)
#define PCI_CMD_SERR_ENABLE (1 << 8)
#define PCI_CMD_PARITY_ERROR_RESPONSE (1 << 6)
#define PCI_CMD_VGA_PALETTE_SNOOP (1 << 5)
#define PCI_CMD_MEMORY_WRITE_INVALIDATE_ENABLE (1 << 4)
#define PCI_CMD_SPECIAL_CYCLES (1 << 3)
#define PCI_CMD_BUS_MASTER (1 << 2)
#define PCI_CMD_MEMORY_SPACE (1 << 1)
#define PCI_CMD_IO_SPACE (1 << 0)

#define PCI_STATUS_CAPABILITIES (1 << 4)

#define PCI_CLASS_UNCLASSIFIED 0x0
#define PCI_CLASS_STORAGE 0x1
#define PCI_CLASS_NETWORK 0x2
#define PCI_CLASS_DISPLAY 0x3
#define PCI_CLASS_MULTIMEDIA 0x4
#define PCI_CLASS_MEMORY 0x5
#define PCI_CLASS_BRIDGE 0x6
#define PCI_CLASS_COMMUNICATION 0x7
#define PCI_CLASS_PERIPHERAL 0x8
#define PCI_CLASS_INPUT_DEVICE 0x9
#define PCI_CLASS_DOCKING_STATION 0xA
#define PCI_CLASS_PROCESSOR 0xB
#define PCI_CLASS_SERIAL_BUS 0xC
#define PCI_CLASS_WIRELESS_CONTROLLER 0xD
#define PCI_CLASS_INTELLIGENT_CONTROLLER 0xE
#define PCI_CLASS_SATELLITE_COMMUNICATION 0xF
#define PCI_CLASS_ENCRYPTON 0x10
#define PCI_CLASS_SIGNAL_PROCESSING 0x11

#define PCI_CLASS_COPROCESSOR 0x40

#define PCI_SUBCLASS_IDE 0x1
#define PCI_SUBCLASS_FLOPPY 0x2
#define PCI_SUBCLASS_ATA 0x5
#define PCI_SUBCLASS_SATA 0x6
#define PCI_SUBCLASS_NVM 0x8

#define PCI_SUBCLASS_ETHERNET 0x0

#define PCI_SUBCLASS_VGA_COMPATIBLE 0x0
#define PCI_SUBCLASS_XGA 0x1

#define PCI_SUBCLASS_AC97 0x1

#define PCI_SUBCLASS_USB 0x3

#define PCI_PROGIF_UHCI 0x20
#define PCI_PROGIF_OHCI 0x10
#define PCI_PROGIF_EHCI 0x20
#define PCI_PROGIF_XHCI 0x30

#define PCI_CAP_MSI_ADDRESS_BASE 0xFEE00000
#define PCI_CAP_MSI_CONTROL_64 (1 << 7)             // 64-bit address capable
#define PCI_CAP_MSI_CONTROL_VECTOR_MASKING (1 << 8) // Enable Vector Masking
#define PCI_CAP_MSI_CONTROL_MME_MASK (0x7U << 4)
#define PCI_CAP_MSI_CONTROL_SET_MME(x)                                         \
    ((x & 0x7) << 4)                                // Multiple message enable
#define PCI_CAP_MSI_CONTROL_MMC(x) ((x >> 1) & 0x7) // Multiple Message Capable
#define PCI_CAP_MSI_CONTROL_ENABLE (1 << 0)         // MSI Enable

#define PCI_PACKAGE_ADDRESS(bus, slot, func, offset)                           \
    ((bus << 16 | slot << 11 | func << 8 | offset & 0xFC | 0x80000000))

namespace PCI {
    enum class ConfigRegs : u8
    {
        VENDOR_ID               = 0x00,
        DEVICE_ID               = 0x02,
        COMMAND                 = 0x04,
        STATUS                  = 0x06,
        REVISION_ID             = 0x08,
        PROG_IF                 = 0x09,
        SUBCLASS                = 0x0A,
        Class                   = 0x0B,
        CacheLineSize           = 0x0C,
        LatencyTimer            = 0x0D,
        HeaderType              = 0x0E,
        BIST                    = 0x0F,
        BAR0                    = 0x10,
        BAR1                    = 0x14,
        BAR2                    = 0x18,
        BAR3                    = 0x1C,
        BAR4                    = 0x20,
        BAR5                    = 0x24,
        CardbusCISPointer       = 0x28,
        SubsystemVendorID       = 0x2C,
        SubsystemID             = 0x2E,
        ExpansionROMBaseAddress = 0x30,
        CapabilitiesPointer     = 0x34,
        InterruptLine           = 0x3C,
        InterruptPin            = 0x3D,
        MinGrant                = 0x3E,
        MaxLatency              = 0x3F
    };

    enum ConfigAccessMode
    {
        Legacy,
        Enhanced
    };

    enum class ClassCode : u8
    {
        UNCLASSIFIED                  = 0x00,
        MASS_STORAGE                  = 0x01,
        NETWORK                       = 0x02,
        DISPLAY                       = 0x03,
        MULTIMEDIA                    = 0x04,
        MEMORY                        = 0x05,
        BRIDGE                        = 0x06,
        COMMUNICATION                 = 0x07,
        PERIPHERAL                    = 0x08,
        INPUT_DEVICE                  = 0x09,
        DOCKING_STATION               = 0x0A,
        PROCESSOR                     = 0x0B,
        SERIAL_BUS                    = 0x0C,
        WIRELESS                      = 0x0D,
        INTELLIGENT                   = 0x0E,
        SATELLITE                     = 0x0F,
        ENCRYPTION                    = 0x10,
        SIGNAL_PROCESSING             = 0x11,
        ACCELERATOR                   = 0x12,
        NON_ESSENTIAL_INSTRUMENTATION = 0x13,
        COPROCESSOR                   = 0x40,
        UNASSIGNED                    = 0xFF
    };

    namespace Subclass {
        enum class Unclassified : u8
        {
            NON_VGA_COMPATIBLE = 0x00,
            VGA_COMPATIBLE     = 0x01
        };

        enum class MassStorage : u8
        {
            SCSI        = 0x00,
            IDE         = 0x01,
            FLOPPY_DISK = 0x02,
            IPI_BUS     = 0x03,
            RAID        = 0x04,
            ATA         = 0x05,
            SATA        = 0x06,
            SAS         = 0x07,
            NVM         = 0x08,
            OTHERS      = 0x80
        };

        enum class Network : u8
        {
            ETHERNET   = 0x00,
            TOKEN_RING = 0x01,
            FDDI       = 0x02,
            ATM        = 0x03,
            ISDN       = 0x04,
            WORLD_FIP  = 0x05,
            PICMG      = 0x06,
            INFINIBAND = 0x07,
            FABRIC     = 0x08,
            OTHERS     = 0x80
        };

        enum class Display : u8
        {
            VGA_COMPATIBLE  = 0x00,
            XGA             = 0x01,
            THREE_DIMENSION = 0x02,
            OTHERS          = 0x80
        };

        enum class Multimedia : u8
        {
            VIDEO     = 0x00,
            AUDIO     = 0x01,
            TELEPHONY = 0x02,
            DEVICE    = 0x03,
            OTHERS    = 0x80
        };

        enum class Memory : u8
        {
            RAM    = 0x00,
            FLASH  = 0x01,
            OTHERS = 0x80
        };

        enum class Bridge : u8
        {
            HOST             = 0x00,
            ISA              = 0x01,
            EISA             = 0x02,
            MCA              = 0x03,
            PCI_TO_PCI       = 0x04,
            PCMCIA           = 0x05,
            NUBUS            = 0x06,
            CARD_BUS         = 0x07,
            RACEWAY          = 0x08,
            SEMI_TRANSPARENT = 0x09,
            INFINIBAND       = 0x0A,
            OTHERS           = 0x80
        };

        enum class Communication : u8
        {
            SERIAL     = 0x00,
            PARALLEL   = 0x01,
            MULTIPORT  = 0x02,
            MODEM      = 0x03,
            GPIB       = 0x04,
            SMART_CARD = 0x05,
            OTHERS     = 0x80
        };

        enum class Peripheral : u8
        {
            PIC         = 0x00,
            DMA         = 0x01,
            TIMER       = 0x02,
            RTC         = 0x03,
            PCI_HOTPLUG = 0x04,
            SD_HOST     = 0x05,
            IOMMU       = 0x06,
            OTHERS      = 0x80
        };

        enum class InputDevice : u8
        {
            KEYBOARD  = 0x00,
            DIGITIZER = 0x01,
            MOUSE     = 0x02,
            SCANNER   = 0x03,
            GAMEPORT  = 0x04,
            OTHERS    = 0x80
        };

        enum class DockingStation : u8
        {
            DOCKING = 0x00,
            OTHERS  = 0x80
        };

        enum class Processor : u8
        {
            CPU386      = 0x00,
            CPU486      = 0x01,
            PENTIUM     = 0x02,
            PENTIUM_PRO = 0x03,
            ALPHA       = 0x10,
            POWERPC     = 0x20,
            MIPS        = 0x30,
            COPROCESSOR = 0x40,
            OTHERS      = 0x80
        };

        enum class SerialBus : u8
        {
            FIREWIRE   = 0x00,
            ACCESS_BUS = 0x01,
            SSA        = 0x02,
            USB        = 0x03,
            FIBRE      = 0x04,
            SMBUS      = 0x05,
            INFINIBAND = 0x06,
            IPMI       = 0x07,
            SERCOS     = 0x08,
            CANBUS     = 0x09,
            OTHERS     = 0x80
        };

        enum class Wireless : u8
        {
            IRDA        = 0x00,
            CONSUMER_IR = 0x01,
            RF          = 0x10,
            BLUETOOTH   = 0x11,
            BROADBAND   = 0x12,
            ETHERNET    = 0x20,
            OTHERS      = 0x80
        };

        enum class Intelligent : u8
        {
            I2O    = 0x00,
            OTHERS = 0x80
        };

        enum class Satellite : u8
        {
            TV     = 0x00,
            AUDIO  = 0x01,
            VOICE  = 0x02,
            DATA   = 0x03,
            OTHERS = 0x80
        };

        enum class Encryption : u8
        {
            NETWORK       = 0x00,
            ENTERTAINMENT = 0x10,
            OTHERS        = 0x80
        };

        enum class SignalProcessing : u8
        {
            DPIO                = 0x00,
            PERFORMANCE_COUNTER = 0x01,
            COMMUNICATION       = 0x10,
            OTHERS              = 0x80
        };
    }
} // namespace PCI