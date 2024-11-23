#pragma once

#include <mixins/std/c++types.h>

namespace AHCI::Command {
    struct HostToDevice
    {
        u8 _fisType;

        u8 _portMultiplier : 4;
        u8 __reserved__0 : 3;
        u8 _mode : 1;

        u8 _command;
        u8 _featLow;

        struct
        {
            u8 _lba0;
            u8 _lba1;
            u8 _lba2;
            u8 _device;
        };

        struct
        {
            u8 _lba3;
            u8 _lba4;
            u8 _lba5;
            u8 _featHigh;
        };

        struct
        {
            u8 _countLow;
            u8 _countHigh;
            u8 _icc;
            u8 _ctl;
        };

        u8 __reserved__1[4];
    } __attribute__((packed));

    struct DeviceToHost
    {
        u8 _fisType;

        u8 _portMultiplier : 4;
        u8 __reserved__0 : 2;
        u8 _interrupt : 1;
        u8 __reserved__1 : 1;

        u8 _statusRegister;
        u8 _errorRegister;

        struct
        {
            u8 _lba0;
            u8 _lba1;
            u8 _lba2;
            u8 _device;
        };

        struct
        {
            u8 _lba3;
            u8 _lba4;
            u8 _lba5;
            u8 __reserved__2;
        };

        struct
        {
            u8 _countLow;
            u8 _countHigh;
            u8 __reserved__3[2];
        };

        u8 __reserved__4[4];
    } __attribute__((packed));

    struct Bidirectional
    {
        u8 _fisType;

        u8 _portMultiplier : 4;
        u8 __reserved__0 : 4;

        u8 __reserved__1[2];

        u32 _data[1];
    } __attribute__((packed));

    struct SetupPIO
    {
        u8 _fisType;

        u8 _portMultiplier : 4;
        u8 __reserved__0 : 1;
        u8 _interrupt : 1;
        u8 __reserved__1 : 1;
        u8 _status : 1;

        u8 _error;

        struct
        {
            u8 _lba0;
            u8 _lba1;
            u8 _lba2;
            u8 _device;
        };

        struct
        {
            u8 _lba3;
            u8 _lba4;
            u8 _lba5;
            u8 __reserved__2;
        };

        struct
        {
            u8 _countLow;
            u8 _countHigh;
            u8 __reserved__3;
            u8 _statusNew;
        };

        struct
        {
            u16 _transferCount;
            u8  _reserved4[2];
        };
    } __attribute__((packed));

    struct SetupDMA
    {
        u8 _fisType;

        u8 _portMultiplier : 4;
        u8 __reserved__0 : 1;
        u8 _interrupt : 1;
        u8 _autoActivate : 1;
        u8 __reserved__1 : 1;

        u8 __reserved__2[2];

        u64 _dmaBufferId;

        u32 __reserved__3;

        u32 _dmaBufferOffset;

        u32 _transferCount;

        u32 __reserved__4;
    } __attribute__((packed));
}