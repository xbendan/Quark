#include <drivers/storage/ide/ide_storage_device.h>
#include <mixins/math/compute.h>
#include <quark/dev/storage/storage_device_partition.h>
#include <quark/hal/ports.h>

namespace IDE {
    using namespace Quark::System::Hal;
    using Quark::System::Io::StorageDevicePartition;

    StorageDevice::StorageDevice(IDE::DeviceType       deviceType,
                                 IDE::DriveInfo const& info,
                                 IDE::DrivePort const& ports)
        : Io::StorageDevice(StringView{ (char const*)info._model,
                                        strnlen((char const*)info._model, 40) },
                            512,
                            info._size / 512)
        , m_deviceType(deviceType)
        , m_info(Std::move(info))
        , m_ports(Std::move(ports))
    {
    }

    bool StorageDevice::Access(Io::Direction direction,
                               u32           sector,
                               u8            count,
                               Bytes         buf)
    {
        out<u8>(m_ports._hddevsel,
                0xE0 | (m_info._drive << 4) | ((sector >> 24) & 0x0F));
        out<u8>(m_ports._seccount0, count);
        out<u8>(m_ports._lba0, sector & 0xFF);
        out<u8>(m_ports._lba1, (sector >> 8) & 0xFF);
        out<u8>(m_ports._lba2, (sector >> 16) & 0xFF);
        out<u8>(m_ports._command,
                Std::toUnderlying(direction == Io::ACCESS_READ
                                      ? Command::READ_PIO
                                      : Command::WRITE_PIO));

        while (in<u8>(m_ports._status) & SR_BUSY)
            ;

        if (direction == Io::ACCESS_READ) {
            in<u16>(m_ports._data, (u16*)buf.buf(), sector * count / 2);
        } else {
            out<u16>(m_ports._data, (u16*)buf.buf(), sector * count / 2);
            out<u8>(m_ports._command, Std::toUnderlying(Command::CACHE_FLUSH));

            while (in<u8>(m_ports._status) & SR_BUSY)
                ;
        }

        if (in<u8>(m_ports._status) & SR_ERROR) {
            return false;
        }

        return true;
    }

    Res<usize> StorageDevice::Read(u64 offset, Bytes bytes)
    {
        if (bytes.len() % SECTOR_SIZE != 0) {
            return ::Error::InvalidArgument(
                "Read buffer size must be a multiple of sector size");
        }

        u32 count  = bytes.len() / SECTOR_SIZE;
        u32 sector = offset / SECTOR_SIZE;
        for (u32 i = 0; i < count; i += 256) {
            u32 readCount = min(count - i, 256u);
            if (!Access(Io::ACCESS_READ,
                        sector + i,
                        readCount,
                        { bytes.buf() + i, readCount * SECTOR_SIZE })) {
                return ::Error::DeviceFault(
                    "Failed to read from storage device, busy or error");
            }
        }

        return Ok(bytes.len());
    }

    Res<usize> StorageDevice::Write(u64 offset, Bytes bytes)
    {
        if (bytes.len() % SECTOR_SIZE != 0) {
            return ::Error::InvalidArgument(
                "Write buffer size must be a multiple of sector size");
        }

        u32 count  = bytes.len() / SECTOR_SIZE;
        u32 sector = offset / SECTOR_SIZE;
        for (u32 i = 0; i < count; i += 256) {
            u32 writeCount = min(count - i, 256u);
            if (!Access(Io::ACCESS_WRITE,
                        sector + i,
                        writeCount,
                        { bytes.buf() + i, writeCount * SECTOR_SIZE })) {
                return ::Error::DeviceFault(
                    "Failed to write to storage device, busy or error");
            }
        }

        return Ok(0ul);
    }
}