#pragma once

#include <mixins/std/c++types.h>

// Reference: https://wiki.osdev.org/PCI_IDE_Controller

namespace IDE {
    enum class DeviceType
    {
        ATA,
        ATAPI
    };

    enum Status
    {
        SR_BUSY          = 0x80,
        SR_DRIVE_READY   = 0x40,
        SR_WRITE_FAULT   = 0x20,
        SR_SEEK_COMPLETE = 0x10,
        SR_DATA_READY    = 0x08,
        SR_CORRECTED     = 0x04,
        SR_INDEX         = 0x02,
        SR_ERROR         = 0x01,
    };

    enum Error
    {
        ER_BAD_BLOCK            = 0x80,
        ER_BAD_DATA             = 0x40,
        ER_MEDIA_CHANGED        = 0x20,
        ER_ID_NOT_FOUND         = 0x10,
        ER_MEDIA_CHANGE_REQUEST = 0x08,
        ER_COMMAND_ABORTED      = 0x04,
        ER_TRACK_ZERO_ERROR     = 0x02,
        ER_NO_ADDRESS_MARK      = 0x01,
    };

    enum class Command
    {
        READ_PIO        = 0x20,
        READ_PIO_EXT    = 0x24,
        READ_DMA        = 0xC8,
        READ_DMA_EXT    = 0x25,
        WRITE_PIO       = 0x30,
        WRITE_PIO_EXT   = 0x34,
        WRITE_DMA       = 0xCA,
        WRITE_DMA_EXT   = 0x35,
        CACHE_FLUSH     = 0xE7,
        CACHE_FLUSH_EXT = 0xEA,
        PACKET          = 0xA0,
        IDENTIFY_PACKET = 0xA1,
        IDENTIFY        = 0xEC,
        // ATAPI commands
        PI_READ         = 0xA8,
        PI_EJECT        = 0x1B,
    };

    enum IdentificationSpaceType
    {
        IDENT_DEVICE_TYPE  = 0,
        IDENT_CYLINDERS    = 2,
        IDENT_HEADS        = 6,
        IDENT_SECTORS      = 12,
        IDENT_SERIAL       = 20,
        IDENT_MODEL        = 54,
        IDENT_CAPABILITIES = 98,
        IDENT_FIELD_VALID  = 106,
        IDENT_MAX_LBA      = 120,
        IDENT_COMMAND_SETS = 164,
        IDENT_MAX_LBA_EXT  = 200,
    };

    enum Protocol : u16
    {
        PROTOCOL_ATA    = 0x00,
        PROTOCOL_ATAPI  = 0x01,
        PROTOCOL_SATA   = 0x02,
        PROTOCOL_SATAPI = 0x03,
    };

    enum Channel : u8
    {
        CHANNEL_PRIMARY   = 0x00,
        CHANNEL_SECONDARY = 0x01,
    };

    enum Drive : u8
    {
        DRIVE_MASTER = 0x00,
        DRIVE_SLAVE  = 0x01,
    };

    enum
    {
        DRIVE_TYPE_MASTER = 0x00,
        DRIVE_TYPE_SLAVE  = 0x01,
    };

    enum RegisterOffset
    {
        REG_DATA       = 0x00,
        REG_ERROR      = 0x01,
        REG_FEATURES   = 0x01,
        REG_SECCOUNT0  = 0x02,
        REG_LBA0       = 0x03,
        REG_LBA1       = 0x04,
        REG_LBA2       = 0x05,
        REG_HDDEVSEL   = 0x06,
        REG_COMMAND    = 0x07,
        REG_STATUS     = 0x07,
        REG_SECCOUNT1  = 0x08,
        REG_LBA3       = 0x09,
        REG_LBA4       = 0x0A,
        REG_LBA5       = 0x0B,
        REG_CONTROL    = 0x0C,
        REG_ALTSTATUS  = 0x0C,
        REG_DEVADDRESS = 0x0D,
    };

    struct ChannelRegisters
    {
        u16 _base;
        u16 _ctrl;
        u16 _busMasterIDE;
        u8  _nIEN{ 0 };
    };

    struct DriveInfo
    {
        u8       __reserved__0{ 1 };
        Channel  _channel; // 0x00 = Primary, 0x01 = Secondary
        Drive    _drive;   // 0x00 = Master, 0x01 = Slave
        Protocol _protocol;
        u16      _signature;
        u16      _capabilities;
        u32      _commands;
        u32      _size;
        u8       _model[41];
    };

    struct DrivePort
    {
        u16 _data;
        u16 _error;
        u16 _features;
        u16 _seccount0;
        u16 _lba0;
        u16 _lba1;
        u16 _lba2;
        u16 _hddevsel;
        u16 _command;
        u16 _status;
        u16 _seccount1{ 0 };
        u16 _lba3{ 0 };
        u16 _lba4{ 0 };
        u16 _lba5{ 0 };
        u16 _control{ 0 };
        u16 _altstatus{ 0 };
        u16 _devaddress{ 0 };
    };
}