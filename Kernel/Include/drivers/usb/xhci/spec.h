#include <drivers/pci/device.h>
#include <mixins/std/c++types.h>

#define XHCI_PORT_OFFSET 0x400

#define USB_CMD_RS (1 << 0)    // Run/Stop
#define USB_CMD_HCRST (1 << 1) // Host Controller Reset
#define USB_CMD_INTE (1 << 2)  // Interrupter enable
#define USB_CMD_HSEE (1 << 3)  // Host System Error enable

#define USB_STS_HCH (1 << 0)  // HCHalted - 0 if CMD_RS is 1
#define USB_STS_HSE (1 << 2)  // Host System Error - set to 1 on error
#define USB_STS_EINT (1 << 3) // Event Interrupt
#define USB_STS_PCD (1 << 4)  // Port change detect
#define USB_STS_SSS (1 << 8)  // Save State Status - 1 when CMD_CSS is 1
#define USB_STS_RSS (1 << 9)  // Restore State Status - 1 when CMD_CRS is 1
#define USB_STS_SRE                                                            \
    (1 << 10) // Save/Restore Error - 1 when error during save or restore
              // operation
#define USB_STS_CNR (1 << 11) // Controller Not Ready - 0 = Ready, 1 = Not Ready
#define USB_STS_HCE (1 << 12) // Host Controller Error

#define USB_CFG_MAXSLOTSEN (0xFF) // Max slots enabled
#define USB_CFG_U3E (1 << 8)      // U3 Entry Enable
#define USB_CFG_CIE (1 << 9)      // Configuration Information Enable

#define USB_CCR_RCS (1 << 0) // Ring Cycle State
#define USB_CCR_CS (1 << 1)  // Command Stop
#define USB_CCR_CA (1 << 2)  // Command Abort
#define USB_CCR_CRR (1 << 3) // Command Ring Running

#define USB_CCR_PTR_LO 0xFFFFFFC0‬
#define USB_CCR_PTR 0xFFFFFFFFFFFFFFC0‬ // Command Ring Pointer

#define XHCI_PORTSC_CCS (1 << 0) // Current Connect Status
#define XHCI_PORTSC_PED (1 << 1) // Port Enabled/Disabled
#define XHCI_PORTSC_OCA (1 << 3) // Overcurrent Active
#define XHCI_PORTSC_PR                                                         \
    (1 << 4) // Port Reset - When software writes a ‘1’ to this bit generating
             // a ‘0’ to ‘1’ transition, the bus reset sequence is initiated
#define XHCI_PORTSC_PP (1 << 9)   // Port Power - When 0 the port is powered off
#define XHCI_PORTSC_CSC (1 << 17) // Connect Status Change
#define XHCI_PORTSC_PEC (1 << 18) // Port Enabled/Disabled Change
#define XHCI_PORTSC_PRC (1 << 21) // Port Reset Change
#define XHCI_PORTSC_WPR (1 << 31) // On USB3 ports warm reset

#define XHCI_INT_ERDP_BUSY (1 << 3)

#define XHCI_TRB_SIZE 16
#define XHCI_EVENT_RING_SEGMENT_TABLE_ENTRY_SIZE 16

namespace Quark::System::Hal::Specs {
    enum class XhciExtendedCapabilities
    {
        LegacySupport                     = 1,
        SupportedProtocol                 = 2,
        ExtendedPowerManagement           = 3,
        IoVirtualization                  = 4,
        MessageSignaledInterrupts         = 5,
        LocalMemory                       = 6,
        DebugCapability                   = 10,
        DoorbellArray                     = 11,
        HostCapabilities                  = 12,
        DeviceCapabilities                = 13,
        USBDebugCapability                = 14,
        ExtendedMessageSignaledInterrupts = 15,
        SecondaryPCIeCapability           = 16,
        MultiHostCapability               = 17,
    };

    struct XhciCapabilitiesRegs
    {
        u8  _capRegLength;
        u8  _reserved;
        u16 _hciVersion;
        u32 _hcsParams1;
        u32 _hcsParams2;
        u32 _hcsParams3;

        union
        {
            u32 _hccParams1;

            struct
            {
                u32 _64bitsCapability : 1;
                u32 _bwNegotationCapability : 1;
                u32 _contextSize : 1;
                u32 _portPowerControl : 1;
                u32 _portIndicators : 1;
                u32 _lightHCResetCapability : 1;
                u32 _latencyToleranceMessagingCapability : 1;
                u32 _noSecondarySIDSupport : 1;
                u32 _parseAllEventData : 1;
                u32 _shortPacketCapability : 1;
                u32 _stoppedEdtlaCapability : 1;
                u32 _contiguousFrameIDCapability : 1;
                u32 _maxPrimaryStreamArraySize : 4;
                u32 _xhciExtendedCapabilitiesPointer : 16;
            } __attribute__((packed));
        };
    };

    struct XhciOperationalRegs
    {
        u32 _usbCommand;
        u32 _usbStatus;
        u32 _pageSize;
        u32 __reserved__0;
        u64 _deviceNotificationControl;

        union
        {
            u64 _commandRingControl;

            struct
            {
                u64 _ringCycleState : 1;
                u64 _ringCmdStop : 1;
                u64 _ringCmdAbort : 1;
                u64 _ringCmdRunning : 1;
                u64 __reserved__1 : 2;
                u64 _ringCmdPtr : 58;
            } __attribute__((packed));
        };

        u8  _reserved2[16];
        u64 _deviceContextBaseAddressArrayPointer;
        u64 _config;
    };
} // namespace Quark::System::Hal::Specs