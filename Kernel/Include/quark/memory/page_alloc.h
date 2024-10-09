#pragma once

#include <mixins/meta/opt.h>
#include <mixins/meta/result.h>
#include <mixins/utils/array.h>
#include <mixins/utils/linked_queue.h>
#include <quark/memory/address_range.h>
#include <quark/memory/address_space.h>
#include <quark/memory/page.h>
#include <quark/memory/page_queue.h>

#define BUDDY_LEVEL_UPPER_LIMIT 10
#define BUDDY_LEVEL_LOWER_LIMIT 0

namespace Quark::System::Memory {
    extern Array<AddressRange[256]>                         g_pageRanges;
    extern Array<PhysMemQueue[BUDDY_LEVEL_UPPER_LIMIT + 1]> g_pageQueues;
    extern PhysMemFrame**                                   g_pageFrames;

    Res<u64> Allocate(usize amount);
    Res<>    Free(u64 address);

    /**
     * @brief
     *
     * @param amount
     * @param addressSpace
     * @param flags
     * @return Res<u64>
     */
    Res<u64> AllocateMemory4K(
        usize                amount,
        AddressSpace*        addressSpace,
        Flags<Hal::VmmFlags> flags = (Hal::VmmFlags::WRITABLE |
                                      Hal::VmmFlags::PRESENT));

    /**
     * @brief
     *
     * @param address
     * @param amount
     * @param addressSpace
     * @return Res<>
     */
    Res<> FreeMemory4K(usize address, usize amount, AddressSpace* addressSpace);

    /**
     * @brief
     *
     * @param phys
     * @param virt
     * @param amount
     * @param addressSpace
     * @param flags
     * @return Res<>
     */
    Res<> MapAddress(u64                  phys,
                     u64                  virt,
                     usize                amount,
                     AddressSpace*        addressSpace,
                     Flags<Hal::VmmFlags> flags);

    /**
     * @brief
     *
     * @param amount
     * @return Res<PhysMemFrame*>
     */
    Res<PhysMemFrame*> AllocatePhysFrame4K(usize amount);

    /**
     * @brief
     *
     * @param amount
     * @return Res<u64>
     */
    Res<u64> AllocatePhysMemory4K(usize amount);

    /**
     * @brief
     *
     * @param address
     * @param amount
     * @return Res<>
     */
    Res<> FreePhysMemory4K(usize address, usize amount);

    /**
     * @brief
     *
     * @param amount
     * @param addressSpace
     * @param flags
     * @return Res<u64>
     */
    Res<u64> AllocateVirtMemory4K(
        usize                amount,
        AddressSpace*        addressSpace,
        Flags<Hal::VmmFlags> flags = (Hal::VmmFlags::WRITABLE |
                                      Hal::VmmFlags::PRESENT));

    /**
     * @brief
     *
     * @param address
     * @param amount
     * @param addressSpace
     * @return Res<>
     */
    Res<> FreeVirtMemory4K(usize         address,
                           usize         amount,
                           AddressSpace* addressSpace);

    Res<> ConvertVirtToPhys(u64 address);
    Res<> ConvertVirtToIO(u64 address);
} // namespace Quark::System::Memory