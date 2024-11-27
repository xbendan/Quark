#include <drivers/pci/device.h>
#include <drivers/storage/ide/ide_storage_device.h>
#include <mixins/utils/array.h>
#include <quark/dev/storage/storage_controller_device.h>

namespace IDE {
    using Qk::Array;
    using Qk::NonnullRefPtr;
    using namespace Quark::System;

    class StorageControllerDevice final
        : public Io::StorageControllerDevice
        , public PCI::Device
    {
    public:
        StorageControllerDevice(PCI::DeviceIdentifier&,
                                u32 bar0 = BAR0,
                                u32 bar1 = BAR1,
                                u32 bar2 = BAR2,
                                u32 bar3 = BAR3,
                                u32 bar4 = BAR4);
        virtual ~StorageControllerDevice() = default;

        virtual Res<> OnInitialize() final;

        virtual Res<NonnullRefPtr<Io::StorageDevice>> GetStorageDevice(
            u8) final;
        void WriteChannel(Channel, RegisterOffset, u8);
        u8   ReadChannel(Channel, RegisterOffset);

        void IdentifyChannel(Channel);
        void IdentifyDrive(Channel, Drive, DrivePort const&);

        void SendCommand(Channel, Command);

    private:
        static inline constexpr u32 BAR0 = 0x1F0;
        static inline constexpr u32 BAR1 = 0x3F6;
        static inline constexpr u32 BAR2 = 0x170;
        static inline constexpr u32 BAR3 = 0x376;
        static inline constexpr u32 BAR4 = 0;

        u32                        m_bars[5];
        Array<ChannelRegisters, 2> m_channels;
        Array<StorageDevice*, 4>   m_devices;
    };
}