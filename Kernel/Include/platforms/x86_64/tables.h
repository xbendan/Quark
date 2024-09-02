#pragma once

#include <mixins/std/c++types.h>

namespace Quark::Platform::X64 {

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
            u8  _baseMid;
            u8  _flags;
            u8  _limitHigh : 4;
            u8  _granularity : 4;
            u8  _baseHigh;

            Entry() {}

            Entry(u32 base, u32 limit, u8 flags, u8 granularity)
                : _limitLow(limit & 0xffff)
                , _baseLow(base & 0xffff)
                , _baseMid((base >> 16) & 0xff)
                , _flags(flags)
                , _limitHigh((limit >> 16) & 0x0f)
                , _granularity(granularity)
                , _baseHigh((base >> 24) & 0xff)
            {
            }
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
            TssEntry(TaskStateSegment* tss);
        } __attribute__((packed)) _tss;

        GlobDescTbl() = delete;

        GlobDescTbl(TaskStateSegment* tss)
            : _entries{ Entry(),
                        Entry(0xaffff,
                              0,
                              (Segment | Present | Executable | ReadWrite),
                              0x0f),
                        Entry(0xaffff,
                              0,
                              (Segment | Present | ReadWrite),
                              0x0f),
                        Entry(
                            0xafff,
                            0,
                            (Segment | Present | Executable | User | ReadWrite),
                            0x0f),
                        Entry(0xafff,
                              0,
                              (Segment | Present | User | ReadWrite),
                              0x0f) }
            , _tss(TssEntry(tss))
        {
        }
    };

    struct TaskStateSegment
    {
        u32 __reserved__0 __attribute__((aligned(0x10)));
        u64 _rsp[3];
        u64 __reserved__1;
        u64 _ist[7];
        u64 __reserved__2;
        u16 __reserved__3;
        u16 _iopb;

        TaskStateSegment() = default;
        TaskStateSegment(GlobDescTbl::TssEntry& tss);
    } __attribute__((packed));

    struct InterruptDescTbl
    {
        static constexpr usize Count = 256;

        enum Flags : u8
        {
            InterruptGate = 0b1000'1110,
            TrapGate      = 0b1110'1111,
            User          = 0b0110'0000,
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

            Entry() {}

            Entry(u64 base, u16 selector, u8 ist, u8 flags)
                : _baseLow(base & 0xffff)
                , _selector(selector)
                , _ist(ist)
                , _flags(flags)
                , _baseMedium((base >> 16) & 0xffff)
                , _baseHigh((base >> 32) & 0xffffffff)
                , __reserved__0(0)
            {
            }
        } __attribute__((packed)) _entries[256];
    } __attribute__((packed));
} // namespace Quark::Platform::X64