#include <quark/memory/address_space.h>

namespace Quark::System::Platform::X64 {
    using namespace Quark::System::Memory;

    template <UserManagement::Level>
    class X64AddressSpace;

    template <>
    class X64AddressSpace<UserManagement::Level::User> : public AddressSpace
    {
    public:
        X64AddressSpace();
        ~X64AddressSpace();

        Res<u64> AllocateVirtPages4K(usize amount, //
                                     Flags<VmmFlags> = VmmFlags::PRESENT |
                                                       VmmFlags::WRITABLE |
                                                       VmmFlags::USER) override;
        Res<u64> AllocateVirtPages2M(usize amount, //
                                     Flags<VmmFlags> = VmmFlags::PRESENT |
                                                       VmmFlags::WRITABLE |
                                                       VmmFlags::USER) override;

        Res<> FreeVirtPages4K(u64   address, //
                              usize amount) override;
        Res<> FreeVirtPages2M(u64   address, //
                              usize amount) override;

        Res<> MapAddress4K(u64             phys, //
                           u64             virt,
                           usize           amount,
                           Flags<VmmFlags> flags) override;
        Res<> MapAddress2M(u64             phys, //
                           u64             virt,
                           usize           amount,
                           Flags<VmmFlags> flags) override;

        Res<Flags<VmmFlags>> getFlags(u64 address) override;
        Res<>                setFlags(u64             address,
                                      Flags<VmmFlags> flags,
                                      bool            set = true) override;
        // Res<Flags<VmmFlags>> setFlagsThenGet(u64 address,
        //                                           Flags<VmmFlags> flags,
        //                                           bool set = true) override;

        Res<u64> GetPhysAddress(u64 address) override;

        pml4_t      _pml4;
        pdpt_t      _pdpt;
        /*
            Theoretically, a program can take up to 512 gigabytes space.
            This is exactly the size of a page directory pointer table, aka
            PDPT, which is 4096 bytes * 512 entries (a page table) * 512 entries
            (a page directory) * 512 entries (a page directory pointer table) =
            512 GB.

            This _pageDirs pointer will actually point to an array of pointers,
            each of which points to a page directory table.
         */
        pagedir_t*  _pageDirs[512];
        /*
            _pageTables is a two-dimensional array of pointers, each of which
            points to a page table. The first dimension is the index of the
            page directory pointer table, and the second dimension is the index
            of the page directory table.
         */
        pagetbl_t** _pageTables[512];

        u64     _pml4Phys;
        u64*    _pdptPhys;
        pdpt_t* _pdptKern;

        void*   _zeroPage;
        /*
            For each user-mode address space, the bitmap is used to track the
            allocated pages. In total, each address space can take up to 512
            gigabytes space, which is 512 * 1024 * 1024 * 1024 / 4096 =
            134,217,728 pages. This means that the bitmap could also take up to
            134,217,728 / 8 = 16,777,216 bytes = 16 megabytes. This is a huge
            amount of memory, consider that not all of the pages are used.
            Therefore, we use a two-dimensional array to track it. Each time we
            allocate 4 pages (16384 bytes) for the record, these pages can track
            about 16384 * 8 = 131,072 pages, hence the 2D array is exactly
            equals to "u64 array[16384 / 8][1024]"
         */
        Bitmap* _bitmap;
    };

    template <>
    class X64AddressSpace<UserManagement::Level::System> : public AddressSpace
    {
    public:
        X64AddressSpace();
        ~X64AddressSpace() = default;

        Res<u64> AllocateVirtPages4K(
            usize amount, //
            Flags<VmmFlags> = VmmFlags::PRESENT | VmmFlags::WRITABLE) override;
        Res<u64> AllocateVirtPages2M(
            usize amount, //
            Flags<VmmFlags> = VmmFlags::PRESENT | VmmFlags::WRITABLE) override;

        Res<> MapAddress4K(u64             phys, //
                           u64             virt,
                           usize           amount,
                           Flags<VmmFlags> flags) override;
        Res<> MapAddress2M(u64             phys, //
                           u64             virt,
                           usize           amount,
                           Flags<VmmFlags> flags) override;

        Res<> FreeVirtPages4K(u64   address, //
                              usize amount) override;
        Res<> FreeVirtPages2M(u64   address, //
                              usize amount) override;

        Res<Flags<VmmFlags>> getFlags(u64 address) override;
        Res<>                setFlags(u64             address, //
                                      Flags<VmmFlags> flags,
                                      bool            set = true) override;
        // Res<Flags<VmmFlags>> setFlagsThenGet(u64 address,
        //                                           Flags<VmmFlags> flags,
        //                                           bool set = true) override;

        Res<u64> GetPhysAddress(u64 address) override;

        pagedir_t*  _pageDirs[512];
        pagetbl_t** _pageTables[512];

        u64 _pml4Phys;
    };
}