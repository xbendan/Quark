#include <mixins/meta/result.h>
#include <mixins/std/c++types.h>

#include <quark/hal/pmm.h>
#include <quark/hal/vmm.h>
#include <quark/memory/address_space.h>
#include <quark/memory/page.h>
#include <quark/privilege/user.h>

namespace Quark::System::API {
    using Quark::System::Memory::AddressSpace;
    using Quark::System::Memory::PhysMemFrame;

    Res<u64> alloc(usize amount);
    Res<>    free(u64 address);

    /**
     * @brief
     *
     * @param amount
     * @param addressSpace
     * @param flags
     * @return Res<u64>
     */
    Res<u64> allocMemory4K(
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
    Res<> freeMemory4K(usize address, usize amount, AddressSpace* addressSpace);

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
    Res<> mapAddress(u64                  phys,
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
    Res<PhysMemFrame*> allocPhysFrame4K(usize amount);

    /**
     * @brief
     *
     * @param amount
     * @return Res<u64>
     */
    Res<u64> allocPhysMemory4K(usize amount);

    /**
     * @brief
     *
     * @param address
     * @param amount
     * @return Res<>
     */
    Res<> freePhysMemory4K(usize address, usize amount);

    /**
     * @brief
     *
     * @param amount
     * @param addressSpace
     * @param flags
     * @return Res<u64>
     */
    Res<u64> allocVirtMemory4K(
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
    Res<> freeVirtMemory4K(usize         address,
                           usize         amount,
                           AddressSpace* addressSpace);

    Res<> convertVirtToPhys(u64 address);
    Res<> convertVirtToIO(u64 address);

    Res<AddressSpace*> createAddressSpace(
        Privilege::Level lv = Privilege::Level::User);
}

#if defined(GLOBAL_API_NAMESPACE)
using namespace Quark::System::API;
#endif