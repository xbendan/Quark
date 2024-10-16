#pragma once

#include <mixins/meta/result.h>
#include <mixins/std/c++types.h>
#include <mixins/utils/array.h>
#include <quark/hal/vmm.h>
#include <quark/privilege/user.h>

#define KERNEL_VIRTUAL_BASE 0xffffffff80000000
#define IO_VIRTUAL_BASE (KERNEL_VIRTUAL_BASE - 0x100000000)

namespace Quark::System::Memory {
    u64 CopyAsIOAddress(u64 address);

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

        virtual Res<u64> Alloc4KPages(
            usize amount, //
            Flags<Hal::VmmFlags> = Hal::VmmFlags::PRESENT |
                                   Hal::VmmFlags::WRITABLE) = 0;
        virtual Res<u64> Alloc2MPages(
            usize amount, //
            Flags<Hal::VmmFlags> = Hal::VmmFlags::PRESENT |
                                   Hal::VmmFlags::WRITABLE) = 0;

        virtual Res<> Free4KPages(u64 address, usize amount) = 0;
        virtual Res<> Free2MPages(u64 address, usize amount) = 0;

        virtual Res<> Map4KPages(u64                  phys, //
                                 u64                  virt,
                                 usize                amount,
                                 Flags<Hal::VmmFlags> flags) = 0;
        virtual Res<> Map2MPages(u64                  phys, //
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