#include <mixins/std/c++types.h>
#include <mixins/utils/array.h>
#include <mixins/utils/bitmap.h>
#include <quark/hal/vmm.h>
#include <quark/memory/address_space.h>
#include <quark/privilege/user.h>

#define VMM_PAGE_ADDRESS_MASK 0x0000fffffffff000
#define VMM_PAGE_ENTRY_COUNT 512

namespace Quark::System::Platform::X64 {
    using namespace Quark::System::Memory;

    template <u8 _Lv>
    class MapLevel
    {
    public:
        union Entry
        {
            u64 _flags;

            struct
            {
                u64 _present : 1;
                u64 _writable : 1;
                u64 _user : 1;
                u64 _writeThrough : 1;
                u64 _cacheDisabled : 1;
                u64 _accessed : 1;
                u64 _dirty : 1;
                u64 _pageSize : 1;
                u64 _global : 1;
                u64 __reserved__2 : 3;
                u64 _address : 40;
                u64 __reserved__3 : 11;
                u64 _disableExecute : 1;
            } __attribute__((packed));

            Entry& withFlags(Hal::VmmFlags flags, bool set = true)
            {
                switch (flags) {
                    case Hal::VmmFlags::PRESENT:
                        _present = set;
                        break;
                    case Hal::VmmFlags::WRITABLE:
                        _writable = set;
                        break;
                    case Hal::VmmFlags::USER:
                        _user = set;
                        break;
                    case Hal::VmmFlags::WRITE_THROUGH:
                        _writeThrough = set;
                        break;
                    case Hal::VmmFlags::CACHE_DISABLED:
                        _cacheDisabled = set;
                        break;
                    case Hal::VmmFlags::ACCESSED:
                        _accessed = set;
                        break;
                    case Hal::VmmFlags::DIRTY:
                        _dirty = set;
                        break;
                    case Hal::VmmFlags::PAGE_SIZE:
                        _pageSize = set;
                        break;
                    case Hal::VmmFlags::GLOBAL:
                        _global = set;
                        break;
                    case Hal::VmmFlags::DISABLE_EXECUTE:
                        _disableExecute = set;
                        break;
                }
                return *this;
            }

            Entry& withFlags(Flags<Hal::VmmFlags> flags, bool set = true)
            {
                _present        = flags & Hal::VmmFlags::PRESENT;
                _writable       = flags & Hal::VmmFlags::WRITABLE;
                _user           = flags & Hal::VmmFlags::USER;
                _writeThrough   = flags & Hal::VmmFlags::WRITE_THROUGH;
                _cacheDisabled  = flags & Hal::VmmFlags::CACHE_DISABLED;
                _accessed       = flags & Hal::VmmFlags::ACCESSED;
                _dirty          = flags & Hal::VmmFlags::DIRTY;
                _pageSize       = flags & Hal::VmmFlags::PAGE_SIZE;
                _global         = flags & Hal::VmmFlags::GLOBAL;
                _disableExecute = flags & Hal::VmmFlags::DISABLE_EXECUTE;

                return *this;
            }

            Entry& clearFlags(Flags<Hal::VmmFlags> flags)
            {
                _present        = !(flags & Hal::VmmFlags::PRESENT);
                _writable       = !(flags & Hal::VmmFlags::WRITABLE);
                _user           = !(flags & Hal::VmmFlags::USER);
                _writeThrough   = !(flags & Hal::VmmFlags::WRITE_THROUGH);
                _cacheDisabled  = !(flags & Hal::VmmFlags::CACHE_DISABLED);
                _accessed       = !(flags & Hal::VmmFlags::ACCESSED);
                _dirty          = !(flags & Hal::VmmFlags::DIRTY);
                _pageSize       = !(flags & Hal::VmmFlags::PAGE_SIZE);
                _global         = !(flags & Hal::VmmFlags::GLOBAL);
                _disableExecute = !(flags & Hal::VmmFlags::DISABLE_EXECUTE);

                return *this;
            }

            Entry& withAddress(u64 address)
            {
                _address = address & VMM_PAGE_ADDRESS_MASK;
                return *this;
            }

        } __attribute__((packed)) _entries[VMM_PAGE_ENTRY_COUNT];

        static_assert(sizeof(MapLevel<_Lv>::Entry) == 8);

        constexpr static usize _len = 512;

        Entry& operator[](usize index) { return _entries[index]; }

        const Entry& operator[](usize index) const { return _entries[index]; }
    };

    using Pml4      = MapLevel<4>; // Page Map Level 4, 256 TiB
    using Pdpt      = MapLevel<3>; // Page Directory Pointer Table, 512 GiB
    using PageDir   = MapLevel<2>; // Page Directory, 1 GiB
    using PageTable = MapLevel<1>; // Page Table, 2 MiB

    template <Privilege::Level>
    class X64AddressSpace;

    template <>
    class X64AddressSpace<Privilege::Level::User> : public AddressSpace
    {
    public:
        X64AddressSpace();
        ~X64AddressSpace();

        Res<u64> alloc4KPages(
            usize amount, //
            Flags<Hal::VmmFlags> = Hal::VmmFlags::PRESENT |
                                   Hal::VmmFlags::WRITABLE |
                                   Hal::VmmFlags::USER) override;
        Res<u64> alloc2MPages(
            usize amount, //
            Flags<Hal::VmmFlags> = Hal::VmmFlags::PRESENT |
                                   Hal::VmmFlags::WRITABLE |
                                   Hal::VmmFlags::USER) override;

        Res<> free4KPages(u64   address, //
                          usize amount) override;
        Res<> free2MPages(u64   address, //
                          usize amount) override;

        Res<> map4KPages(u64                  phys, //
                         u64                  virt,
                         usize                amount,
                         Flags<Hal::VmmFlags> flags) override;
        Res<> map2MPages(u64                  phys, //
                         u64                  virt,
                         usize                amount,
                         Flags<Hal::VmmFlags> flags) override;

        Res<Flags<Hal::VmmFlags>> getFlags(u64 address) override;
        Res<>                     setFlags(u64                  address,
                                           Flags<Hal::VmmFlags> flags,
                                           bool                 set = true) override;
        // Res<Flags<Hal::VmmFlags>> setFlagsThenGet(u64 address,
        //                                           Flags<Hal::VmmFlags> flags,
        //                                           bool set = true) override;

        Res<u64> getPhysAddress(u64 address) override;

        Pml4                     _pml4;
        Pdpt                     _pdpt;
        /*
            Theoretically, a program can take up to 512 gigabytes space.
            This is exactly the size of a page directory pointer table, aka
            PDPT, which is 4096 bytes * 512 entries (a page table) * 512 entries
            (a page directory) * 512 entries (a page directory pointer table) =
            512 GB.

            This _pageDirs pointer will actually point to an array of pointers,
            each of which points to a page directory table.
         */
        Array<PageDir* [512]>    _pageDirs;
        /*
            _pageTables is a two-dimensional array of pointers, each of which
            points to a page table. The first dimension is the index of the
            page directory pointer table, and the second dimension is the index
            of the page directory table.
         */
        Array<PageTable** [512]> _pageTables;

        u64   _pml4Phys;
        Pdpt* _pdptPhys;
        Pdpt* _pdptKern;

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
    class X64AddressSpace<Privilege::Level::System> : public AddressSpace
    {
    public:
        X64AddressSpace();
        ~X64AddressSpace() = default;

        Res<u64> alloc4KPages(
            usize amount, //
            Flags<Hal::VmmFlags> = Hal::VmmFlags::PRESENT |
                                   Hal::VmmFlags::WRITABLE) override;
        Res<u64> alloc2MPages(
            usize amount, //
            Flags<Hal::VmmFlags> = Hal::VmmFlags::PRESENT |
                                   Hal::VmmFlags::WRITABLE) override;

        Res<> map4KPages(u64                  phys, //
                         u64                  virt,
                         usize                amount,
                         Flags<Hal::VmmFlags> flags) override;
        Res<> map2MPages(u64                  phys, //
                         u64                  virt,
                         usize                amount,
                         Flags<Hal::VmmFlags> flags) override;

        Res<> free4KPages(u64   address, //
                          usize amount) override;
        Res<> free2MPages(u64   address, //
                          usize amount) override;

        Res<Flags<Hal::VmmFlags>> getFlags(u64 address) override;
        Res<>                     setFlags(u64                  address, //
                                           Flags<Hal::VmmFlags> flags,
                                           bool                 set = true) override;
        // Res<Flags<Hal::VmmFlags>> setFlagsThenGet(u64 address,
        //                                           Flags<Hal::VmmFlags> flags,
        //                                           bool set = true) override;

        Res<u64> getPhysAddress(u64 address) override;

        Pml4                     _pml4;
        Pdpt                     _pdpt;
        Array<PageDir* [512]>    _pageDirs;
        Array<PageTable** [512]> _pageTables;

        u64 _pml4Phys;
    };

    inline constexpr u16 pml4IndexOf(u64 addr)
    {
        return (addr >> 39) & 0x1FF;
    }

    inline constexpr u16 pdptIndexOf(u64 addr)
    {
        return (addr >> 30) & 0x1FF;
    }

    inline constexpr u16 pdirIndexOf(u64 addr)
    {
        return (addr >> 21) & 0x1FF;
    }

    inline constexpr u16 pageIndexOf(u64 addr)
    {
        return (addr >> 12) & 0x1FF;
    }

} // namespace Quark::System::Platform::X64