#include <drivers/pci/device.h>

namespace Quark::System::Hal {
    struct NVMeControllerRegs
    {
        u64 _capabilities;
        u32 _version;
        u32 _interruptMaskSet;
        u32 _interruptMaskClear;
        u32 _config;
        u32 _reserved0;
        u32 _status;
        u32 _nvmSubsystemReset;
        u32 _adminQueueAttr;
        u64 _adminQueueSubmission;
        u64 _adminQueueCompletion;
        u32 _memBufferLocation;
        u32 _memBufferSize;
        u32 _bootPartInfo;
        u32 _bootPartReadSel;
        u32 _bootPartMemBufferLoc;
        u32 _bootPartMemBufferSize;
        u64 _memBufferSpaceControl;
        u32 _memBufferStatus;
    } __attribute__((packed));

    class NVMeControllerDevice : public PCI::Device
    {};
} // namespace Quark::System::Hal