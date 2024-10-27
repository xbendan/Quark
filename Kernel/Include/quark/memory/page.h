#pragma once

#include <mixins/meta/opt.h>
#include <mixins/meta/result.h>
#include <mixins/std/c++types.h>
#include <mixins/utils/flags.h>
#include <quark/hal/pmm.h>

#define PAGE_SIZE_4K 0x1000ULL
#define PAGE_SIZE_2M 0x200000ULL
#define PAGE_SIZE_1G 0x40000000ULL

#define PAGE_SIZE_PARTITION (PAGE_SIZE_1G / 8)
#define PAGE_BLOCK_AMOUNT (PAGE_SIZE_PARTITION / PAGE_SIZE_4K)
#define PAGE_CHAIN_AMOUNT 0x400
#define PAGE_BIOS_RESERVED 0x100000
#define PAGE_SHIFT 12

namespace Quark::System::Memory {
    struct BuddyPageInfo
    {
        u8  _level;
        u16 _chainLength;
    } __attribute__((packed));
    using buddyinfo_t = BuddyPageInfo;

    struct PageFrame
    {
        PageFrame* _next{};
        PageFrame* _previous{};

        Flags<Hal::PmmFlags> _flags;

        // struct /* buddy */
        // {
        //     u8  _level;
        //     u16 _chainLength{ PAGE_CHAIN_AMOUNT };
        // } __attribute__((packed));
        u8  _level;
        u16 _chainLength;

        struct /* slub */
        {
            struct
            {
                u32 _inuse : 16;
                u32 _objects : 15;
                u32 _frozen : 1;
            } _kmem __attribute__((packed));

            void** _freelist{ nullptr };
        } __attribute__((packed));

        union
        {
            u64        _priv;
            PageFrame* _pageHead;
            void*      _kmemPool;
        };

        u64 _address;

        Optional<PageFrame*> Divide();
        Optional<PageFrame*> Combine(PageFrame* page);
        Optional<PageFrame*> Combine();

        static PageFrame* ByAddress(u64 address);
    };
    static_assert(sizeof(PageFrame) == 48,
                  "Physical memory frame size mismatch");

    static inline bool IsPageAligned(u64 address,
                                     u8  level,
                                     u64 pageSize = PAGE_SIZE_4K)
    {
        return !(address % ((1 << level) * PAGE_SIZE_4K));
    }

    static inline u8 GetSizeAsLevel(usize size)
    {
        u8 level = 0;
        while (size > 1) {
            size >>= 1;
            level++;
        }
        return level;
    }

    static inline u64 GetLevelAsSize(u8 level)
    {
        return (1 << level);
    }

    static inline u64 GetLevelAsOffset(u8 level, u64 offset = PAGE_SIZE_4K)
    {
        return ((1 << level) * offset);
    }
} // namespace Quark::System::Memory