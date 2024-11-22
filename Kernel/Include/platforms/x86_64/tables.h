#pragma once

#define STACK_SIZE 0x4000

#include <mixins/std/c++types.h>

namespace Quark::System::Platform::X64 {

    struct TaskStateSegment;

    struct GlobDescTbl
    {
        enum class Sel
        {
            Null       = 0,
            KernelCode = 1,
            KernelData = 2,
            UserCode   = 3,
            UserData   = 4,
            Tss        = 5
        };

        enum Flags : u32
        {
            Segment    = 0b0001'0000,
            Present    = 0b1000'0000,
            User       = 0b0110'0000,
            Executable = 0b0000'1000,
            ReadWrite  = 0b0000'0010
        };

        struct Pack
        {
            u16 _size;
            u64 _offset;
        } __attribute__((packed));

        struct EntryExtra
        {
            u32 _baseHighest;
            u32 __reserved__0;
        } __attribute__((packed));

        struct Entry
        {
            u16 _limitLow;
            u16 _baseLow;
            u8  _baseMedium;
            /*
             * 0: Accessed
             * 1: Read/Write
             * 2: Direction/Conforming
             * 3: Executable
             * 4: Descriptor Type
             * 5: Privilege Level
             * 6: Present
             */
            u8  _access;
            u8  _granularity;
            u8  _baseHigh;
        } __attribute__((packed)) _entries[5];

        struct TssEntry
        {
            u16 _len;
            u16 _baseLow;
            u8  _baseMedium;
            u8  _flags0;
            u8  _flags1;
            u8  _baseHigh;
            u32 _baseHighest;
            u32 __reserved__0;

            TssEntry() = default;
            TssEntry(TaskStateSegment* tss)
                : _len(108)
                , _baseLow((u64)tss & 0xffff)
                , _baseMedium(((u64)tss >> 16) & 0xff)
                , _flags0(0b10001001)
                , _flags1(0x00)
                , _baseHigh(((u64)tss >> 24) & 0xff)
                , _baseHighest(((u64)tss >> 32) & 0xffffffff)
                , __reserved__0(0)
            {
            }
            TssEntry(u16 len,
                     u16 baseLow,
                     u8  baseMedium,
                     u8  flags0,
                     u8  flags1,
                     u8  baseHigh,
                     u32 baseHighest)
                : _len(len)
                , _baseLow(baseLow)
                , _baseMedium(baseMedium)
                , _flags0(flags0)
                , _flags1(flags1)
                , _baseHigh(baseHigh)
                , _baseHighest(baseHighest)
                , __reserved__0(0)
            {
            }
        } __attribute__((packed)) _tss;

        GlobDescTbl() = delete;

        GlobDescTbl(TaskStateSegment* tss)
            : _entries{
                // { 0x0000, 0x0000, 0x00, 0x00, 0x00,                       0x00},
                // { 0xFFFF, 0x0000, 0x00, 0x9A, (1 << 5) | (1 << 7) | 0x0F, 0x00},
                // { 0xFFFF, 0x0000, 0x00, 0x92, (1 << 5) | (1 << 7) | 0x0F, 0x00},
                // { 0xFFFF, 0x0000, 0x00, 0xFA, (1 << 5) | (1 << 7) | 0x0F, 0x00},
                // { 0xFFFF, 0x0000, 0x00, 0xF2, (1 << 5) | (1 << 7) | 0x0F, 0x00}
                {0xFFFF, 0x0000, 0x00, 0b00000000, 0b00000000, 0x00},
                {0x0000, 0x0000, 0x00, 0b10011010, 0b00100000, 0x00},
                {0x0000, 0x0000, 0x00, 0b10010010, 0b00000000, 0x00},
                {0x0000, 0x0000, 0x00, 0b11111010, 0b00100000, 0x00},
                {0x0000, 0x0000, 0x00, 0b11110010, 0b00000000, 0x00}, 
            }
            , _tss{ 108, 0, 0, 0b10001001, 0b00000000, 0, 0 }
        {
        }
    };

    struct TaskStateSegment
    {
        u32 __reserved__0 __attribute__((aligned(0x10)));
        u64 _rsp[3];
        u64 __reserved__1;
        /* Interrupt Stack Table */
        u64 _ist[7];
        u64 __reserved__2;
        u64 __reserved__3;
        u32 _iopb;

        TaskStateSegment() = default;
        TaskStateSegment(u64 istp);
    } __attribute__((packed));

    struct InterruptDescTbl
    {
        static constexpr usize Count = 256;

        enum Flags : u8
        {
            IntGate  = 0b1000'1110,
            TrapGate = 0b1110'1111,
            User     = 0b0110'0000,
        };

        struct Pack
        {
            u16 _size;
            u64 _offset;
        } __attribute__((packed));

        struct Entry
        {
            u16 _baseLow;
            u16 _selector;
            u8  _ist;
            u8  _flags;
            u16 _baseMedium;
            u32 _baseHigh;
            u32 __reserved__0;

            constexpr Entry() {}

            constexpr Entry(u64 base, u16 selector, u8 flags, u8 ist = 0)
                : _baseLow(base & 0xffff)
                , _selector(selector)
                , _ist(ist & 0x7)
                , _flags(flags)
                , _baseMedium((base >> 16) & 0xffff)
                , _baseHigh((base >> 32) & 0xffffffff)
                , __reserved__0(0)
            {
            }
        } __attribute__((packed)) Entries[256];
    } __attribute__((packed));
} // namespace Quark::System::Platform::X64