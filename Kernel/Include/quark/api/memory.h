#include <mixins/meta/result.h>
#include <mixins/std/c++types.h>

#include <quark/hal/pmm.h>
#include <quark/hal/vmm.h>
#include <quark/memory/address_space.h>
#include <quark/memory/page.h>
#include <quark/privilege/user.h>

namespace Quark::API {
    using Mem::AddressSpace;
    using Mem::PhysMemFrame;

    Res<u64> alloc(usize amount);
    Res<>    free(u64 address);

    Res<u64> allocMemory4K(
        usize                amount,
        AddressSpace*        addressSpace,
        Flags<Hal::VmmFlags> flags = (Hal::VmmFlags::WRITABLE |
                                      Hal::VmmFlags::PRESENT));
    Res<> freeMemory4K(usize address, usize amount, AddressSpace* addressSpace);
    Res<> mapAddress(u64                  phys,
                     u64                  virt,
                     usize                amount,
                     AddressSpace*        addressSpace,
                     Flags<Hal::VmmFlags> flags);

    Res<PhysMemFrame*> allocPhysFrame4K(usize amount);
    Res<u64>           allocPhysMemory4K(usize amount);
    Res<>              freePhysMemory4K(usize address, usize amount);

    Res<u64> allocVirtMemory4K(
        usize                amount,
        AddressSpace*        addressSpace,
        Flags<Hal::VmmFlags> flags = (Hal::VmmFlags::WRITABLE |
                                      Hal::VmmFlags::PRESENT));
    Res<> freeVirtMemory4K(usize         address,
                           usize         amount,
                           AddressSpace* addressSpace);

    Res<> convertVirtToPhys(u64 address);
    Res<> convertVirtToIO(u64 address);

    Res<AddressSpace*> createAddressSpace(
        Privilege::Level lv = Privilege::Level::User);
}

#if defined(GLOBAL_API_NAMESPACE)
using namespace Quark::API;
#endif