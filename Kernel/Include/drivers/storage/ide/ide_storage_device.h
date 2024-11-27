#pragma once

#include <drivers/storage/ide/spec.h>
#include <mixins/str/string_view.h>
#include <quark/dev/storage/storage_device.h>

namespace IDE {
    using namespace Quark::System;
    using Qk::StringView;

    class StorageControllerDevice;

    class StorageDevice final : public Io::StorageDevice
    {
    public:
        StorageDevice(IDE::DeviceType       deviceType,
                      IDE::DriveInfo const& info,
                      IDE::DrivePort const& ports);
        virtual ~StorageDevice() final = default;

        /**
         * @brief
         *
         * @param direction
         * @param sector
         * @param count
         * @param buf
         * @return bool
         */
        bool Access(Io::Direction direction, u32 sector, u8 count, Bytes bytes);
        virtual Res<usize> Read(u64, Bytes) final;
        virtual Res<usize> Write(u64, Bytes) final;

    protected:
        friend class StorageControllerDevice;

        IDE::DeviceType m_deviceType;
        IDE::DriveInfo  m_info;
        IDE::DrivePort  m_ports;
    };
}