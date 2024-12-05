#include <drivers/storage/ide/ide_controller_device.h>
#include <mixins/std/type_traits.h>
#include <quark/hal/ports.h>
#include <quark/os/diagnostic/logging.h>
#include <quark/sched/completable.h>

namespace IDE {
    using namespace Quark::System::Hal;
    using namespace Quark::System::Diagnostic;
    using Qk::StringView;

    StorageControllerDevice::StorageControllerDevice(
        DeviceIdentifier& deviceIdentifier,
        u32               bar0,
        u32               bar1,
        u32               bar2,
        u32               bar3,
        u32               bar4)
        : PCI::Device(deviceIdentifier,
                      "IDE Mass Storage Controller"s,
                      Device::Type::StorageController)
        , m_bars{ bar0, bar1, bar2, bar3, bar4 }
        , m_devices()
    {
    }

    Res<> StorageControllerDevice::OnInitialize()
    {
        m_channels[0] = {
            ._base = (u16)((m_bars[0] & 0xFFFFFFFC) + BAR0 * (!m_bars[0])),
            ._ctrl = (u16)((m_bars[1] & 0xFFFFFFFC) + BAR1 * (!m_bars[1])),
            ._busMasterIDE = (u16)((m_bars[4] & 0xFFFFFFFC) + 0),
        };
        m_channels[1] = {
            ._base = (u16)((m_bars[2] & 0xFFFFFFFC) + BAR2 * (!m_bars[2])),
            ._ctrl = (u16)((m_bars[3] & 0xFFFFFFFC) + BAR3 * (!m_bars[3])),
            ._busMasterIDE = (u16)((m_bars[4] & 0xFFFFFFFC) + 8),
        };

        WriteChannel(CHANNEL_PRIMARY, REG_CONTROL, 0x02);
        WriteChannel(CHANNEL_SECONDARY, REG_CONTROL, 0x02);

        info("[IDE] Identifying primary channel...");
        IdentifyChannel(CHANNEL_PRIMARY);

        info("[IDE] Identifying secondary channel...");
        IdentifyChannel(CHANNEL_SECONDARY);

        info("[IDE] Complete!");
        return Ok();
    }

    Res<NonnullRefPtr<Io::StorageDevice>>
    StorageControllerDevice::GetStorageDevice(u8 index)
    {
        if (index >= 4) {
            return ::Error::IndexOutOfBounds();
        }

        return Ok(NonnullRefPtr<Io::StorageDevice>(m_devices[index]));
    }

    void WaitForReadyStatus(u16 port)
    {
        u8 status = 0;
        do {
            status = in<u8>(port);
        } while (status & SR_BUSY);
    }

    bool WaitForIdentifyData(u16 port)
    {
        u8 status = 0;
        do {
            status = in<u8>(port);
        } while (!(status & SR_DATA_READY) && !(status & SR_ERROR));

        return !(status & SR_ERROR);
    }

    void StorageControllerDevice::IdentifyChannel(Channel channel)
    {
        DrivePort ports = {
            ._data      = (u16)(m_channels[channel]._base + 0),
            ._error     = (u16)(m_channels[channel]._base + 1),
            ._features  = (u16)(m_channels[channel]._base + 1),
            ._seccount0 = (u16)(m_channels[channel]._base + 2),
            ._lba0      = (u16)(m_channels[channel]._base + 3),
            ._lba1      = (u16)(m_channels[channel]._base + 4),
            ._lba2      = (u16)(m_channels[channel]._base + 5),
            ._hddevsel  = (u16)(m_channels[channel]._base + 6),
            ._command   = (u16)(m_channels[channel]._base + 7),
            ._status    = (u16)(m_channels[channel]._base + 7),
        };

        out<u8>(ports._lba0, 0x88);
        if (in<u8>(ports._lba0) != 0x88) {
            return;
        }

        IdentifyDrive(channel, DRIVE_MASTER, ports);
        IdentifyDrive(channel, DRIVE_SLAVE, ports);
    }

    void StorageControllerDevice::IdentifyDrive(Channel          channel,
                                                Drive            drive,
                                                DrivePort const& port)
    {
        info$("[IDE] Identifying {} drive on {} channel...",
              drive == DRIVE_MASTER ? "master" : "slave",
              channel == CHANNEL_PRIMARY ? "primary" : "secondary");

        out<u8>(port._hddevsel, 0xA0 | (drive << 4));
        out<u8>(port._seccount0, 0);
        out<u8>(port._lba0, 0);
        out<u8>(port._lba1, 0);
        out<u8>(port._lba2, 0);
        out<u8>(port._command, Std::toUnderlying(Command::IDENTIFY));

        if (in<u8>(port._status) == 0) {
            return;
        }
        WaitForReadyStatus(port._status);

        if ((in<u8>(port._lba1) != 0 && in<u8>(port._lba2) != 0) ||
            !WaitForIdentifyData(port._status)) {
            return;
        }

        u8 buffer[512] = {};
        in(port._data, (u32*)buffer, 128);

        DriveInfo info{
            ._channel      = channel,
            ._drive        = drive,
            ._protocol     = PROTOCOL_ATA,
            ._signature    = *(u16*)(buffer + IDENT_DEVICE_TYPE),
            ._capabilities = *(u16*)(buffer + IDENT_CAPABILITIES),
            ._commands     = *(u32*)(buffer + IDENT_COMMAND_SETS),
            ._size         = *(u32*)(buffer + IDENT_MAX_LBA) * 512,
            ._model        = { 0 },
        };
        memcpy(info._model, buffer + IDENT_MODEL, 40);

        // Swap the bytes in the model name
        for (u8 i = 0; i < 40; i += 2) {
            u8 tmp             = info._model[i];
            info._model[i]     = info._model[i + 1];
            info._model[i + 1] = tmp;
        }
        // Trim the model name
        for (u8 i = 39; i > 0; i--) {
            if (info._model[i] == ' ') {
                info._model[i] = '\0';
            } else {
                break;
            }
        }

        StringView modelName{
            (char const*)info._model,
            strnlen((const char*)info._model, 40),
        };

        info$("[IDE] Found {} drive: {}, in {} channel, size: {d} bytes",
              drive == DRIVE_MASTER ? "master" : "slave",
              modelName,
              channel == CHANNEL_PRIMARY ? "primary" : "secondary",
              info._size);
        Device::Load(m_devices[channel * 2 + drive] =
                         new StorageDevice(DeviceType::ATA, info, port));
    }

    void StorageControllerDevice::SendCommand(Channel channel, Command command)
    {
        WriteChannel(channel, REG_COMMAND, Std::toUnderlying(command));
    }

    void StorageControllerDevice::WriteChannel(Channel        channel,
                                               RegisterOffset offset,
                                               u8             data)
    {
        if (offset > 0x07 && offset < 0x0C) {
            WriteChannel(
                channel, REG_CONTROL, 0x80 | m_channels[channel]._nIEN);
        }

        if (offset < 0x08)
            out<u8>(m_channels[channel]._base + offset - 0x00, data);
        else if (offset < 0x0C)
            out<u8>(m_channels[channel]._base + offset - 0x06, data);
        else if (offset < 0x0E)
            out<u8>(m_channels[channel]._ctrl + offset - 0x0A, data);
        else
            out<u8>(m_channels[channel]._busMasterIDE + offset - 0x0E, data);

        if (offset > 0x07 && offset < 0x0C) {
            WriteChannel(channel, REG_CONTROL, m_channels[channel]._nIEN);
        }
    }

    u8 StorageControllerDevice::ReadChannel(Channel        channel,
                                            RegisterOffset offset)
    {
        u8 result = 0;

        if (offset > REG_STATUS) {
            WriteChannel(
                channel, REG_CONTROL, 0x80 | m_channels[channel]._nIEN);
        }

        if (offset < 0x08)
            result = in<u8>(m_channels[channel]._base + offset - 0x00);
        else if (offset < 0x0C)
            result = in<u8>(m_channels[channel]._base + offset - 0x06);
        else if (offset < 0x0E)
            result = in<u8>(m_channels[channel]._ctrl + offset - 0x0A);
        else
            result = in<u8>(m_channels[channel]._busMasterIDE + offset - 0x0E);

        if (offset > 0x07 && offset < 0x0C) {
            WriteChannel(channel, REG_CONTROL, m_channels[channel]._nIEN);
        }

        return result;
    }
}