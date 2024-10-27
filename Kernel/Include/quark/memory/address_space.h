#pragma once

#include <mixins/meta/result.h>
#include <mixins/std/c++types.h>
#include <mixins/utils/array.h>
#include <quark/hal/vmm.h>
#include <quark/privilege/user.h>

#define KERNEL_VIRTUAL_BASE 0xffffffff80000000ULL
#define KERNEL_HEAP_BASE (KERNEL_VIRTUAL_BASE + 0x40000000)
#define IO_VIRTUAL_BASE (KERNEL_VIRTUAL_BASE - 0x100000000)

namespace Quark::System::Memory {
    u64 CopyAsIOAddress(u64 address);

    template <typename T>
    inline T* CopyAsIOAddress(T* address)
    {
        return reinterpret_cast<T*>(
            CopyAsIOAddress(reinterpret_cast<u64>(address)));
    }

    class AddressSpace
    {
    public:
        enum Type
        {
            Kernel,
            User,
        };

        AddressSpace()
            : _stats()
        {
        }

        virtual ~AddressSpace() {}

        virtual Res<u64> AllocateVirtPages4K(
            usize amount, //
            Flags<Hal::VmmFlags> = Hal::VmmFlags::PRESENT |
                                   Hal::VmmFlags::WRITABLE) = 0;
        virtual Res<u64> AllocateVirtPages2M(
            usize amount, //
            Flags<Hal::VmmFlags> = Hal::VmmFlags::PRESENT |
                                   Hal::VmmFlags::WRITABLE) = 0;

        virtual Res<> FreeVirtPages4K(u64 address, usize amount) = 0;
        virtual Res<> FreeVirtPages2M(u64 address, usize amount) = 0;

        virtual Res<> MapAddress4K(u64                  phys, //
                                   u64                  virt,
                                   usize                amount,
                                   Flags<Hal::VmmFlags> flags) = 0;
        virtual Res<> MapAddress2M(u64                  phys, //
                                   u64                  virt,
                                   usize                amount,
                                   Flags<Hal::VmmFlags> flags) = 0;

        /**
         * @brief Get the presented flags from input flags
         *
         * @param address
         * @param flags
         * @return Res<Flags<Hal::VmmFlags>>
         */
        // virtual Res<Flags<Hal::VmmFlags>> getFlags(
        //     u64                  address, //
        //     Flags<Hal::VmmFlags> flags)                             = 0;
        /**
         * @brief Get the flags
         *
         * @param address
         * @return Res<Flags<Hal::VmmFlags>>
         */
        virtual Res<Flags<Hal::VmmFlags>> getFlags(u64 address)     = 0;
        /**
         * @brief Set or clear the input flags
         *
         * @param address
         * @param flags
         * @param set
         * @return Res<>
         */
        virtual Res<>                     setFlags(u64                  address,
                                                   Flags<Hal::VmmFlags> flags,
                                                   bool                 set = true) = 0;
        // virtual Res<Flags<Hal::VmmFlags>> setFlagsThenGet(
        //     u64                  address,
        //     Flags<Hal::VmmFlags> flags,
        //     bool                 set = true) = 0;

        virtual Res<bool> IsPresent(u64 address)
        {
            return getFlags(address).Select<bool>(
                [](Flags<Hal::VmmFlags> flags) {
                    return flags.has(Hal::VmmFlags::PRESENT);
                });
        }

        virtual Res<u64> GetPhysAddress(u64) = 0;

        static Res<AddressSpace*> Create(
            Privilege::Level lv = Privilege::Level::User);

    protected:
        struct Stats
        {
            u64 _allocated;
            u64 _swapped;
        } _stats;
    };
} // namespace Quark::System::Memory