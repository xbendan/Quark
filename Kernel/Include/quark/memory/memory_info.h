#pragma once

#include <mixins/std/c++types.h>
#include <mixins/utils/array.h>
#include <quark/memory/address_range.h>

namespace Quark::System::Memory {
    struct MemmapEntry
    {
        AddressRange _range;
        enum Type
        {
            Free,
            Reserved,
            Reclaimable,
            AcpiReclaimable,
            AcpiNvs,
            BadMemory,
            Kernel,
            Module,
            PageTable,
            Stack,
            Heap,
            Data,
            Code,
            Bss,
            Other,
        } _type;
    };

    struct MemoryConfiguration
    {
        u64 _totalSize;
        u64 _availableSize;
        u64 _limit;
        u64 _committed;

        Array<MemmapEntry[256]> _addressRanges;
    };

    struct MemoryAllocationStatus
    {
        MemoryConfiguration& _conf;

        u64 _allocatedPages;
        u64 _freePages;
        u64 _swappedPages;
        u64 _totalPages;
        u64 _pageSize;
    };
}

enum OsMemRangeType
{
    MEM_RANGE_TYPE_FREE,
    MEM_RANGE_TYPE_RESERVED,
    MEM_RANGE_TYPE_ACPI_RECLAIMABLE,
    MEM_RANGE_TYPE_ACPI_NVS,
    MEM_RANGE_TYPE_BAD,
    MEM_RANGE_TYPE_KERNEL,
    MEM_RANGE_TYPE_MODULE,
    MEM_RANGE_TYPE_PAGETABLE,
    MEM_RANGE_TYPE_STACK,
    MEM_RANGE_TYPE_HEAP,
    MEM_RANGE_TYPE_DATA,
    MEM_RANGE_TYPE_CODE,
    MEM_RANGE_TYPE_BSS,
    MEM_RANGE_TYPE_OTHER,
};

struct OsMemRangeInfo
{
    Quark::System::Memory::AddressRange _value;
    OsMemRangeType                      _type;
};

struct OsMemoryConfig
{
    u64 _totalSize;
    u64 _availableSize;
    u64 _limit;
    u64 _committed;

    Array<OsMemRangeInfo[256]> _addressRanges;
    // OsMemRangeInfo _addressRanges[256];
};

struct OsPageInfo
{
    u64 _allocatedPages;
    u64 _freePages;
    u64 _swappedPages;
    u64 _totalPages;
    u64 _pageSize;
};