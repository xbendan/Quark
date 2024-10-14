#pragma once

#include <mixins/meta/func.h>
#include <mixins/std/c++types.h>
#include <mixins/std/string.h>
#include <mixins/utils/flags.h>

namespace Quark::System::Hal {
    struct Platform
    {
        string _name;
        string _version;
        enum class Type : u8
        {
            Unknown,
            BareMetal,
            VirtualMachine,
            Emulator,
            Hypervisor,
            Container,
            Cloud,
            Mobile,
            Desktop,
            Server,
            SuperComputer,
        } _type;
        enum BitWidth : u8
        {
            Unknown,
            Bit16,
            Bit32,
            Bit64,
            Bit128,
        } _bitWidth;
        enum Features : u8
        {
            AddressSpaceIsolation = 1 << 0,
            ProcessContextSwitch  = 1 << 1,
        };
        Flags<Features> _features;
    };
}