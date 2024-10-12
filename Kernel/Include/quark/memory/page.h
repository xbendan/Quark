#pragma once

#include <mixins/meta/opt.h>
#include <mixins/meta/result.h>
#include <mixins/std/c++types.h>
#include <mixins/utils/flags.h>
#include <quark/hal/pmm.h>

#define PAGE_SIZE_4K 4096ULL
#define PAGE_SIZE_2M 209'7152ULL
#define PAGE_SIZE_1G 10'7374'1824ULL

#define PAGE_SIZE_PARTITION (PAGE_SIZE_1G / 8)

namespace Quark::System::Memory {
    struct PhysMemFrame
    {
        PhysMemFrame* _next{};
        PhysMemFrame* _previous{};
        u8            _level;

        Flags<Hal::PmmFlags> _flags;

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
            u64           _priv;
            PhysMemFrame* _pageHead;
            void*         _kmemPool;
        };

        u64 _address;

        Optional<PhysMemFrame*> split();
        Optional<PhysMemFrame*> merge(PhysMemFrame* page);
        Optional<PhysMemFrame*> merge();

        static PhysMemFrame* at(u64 address);
    };

    static_assert(sizeof(PhysMemFrame) == 48,
                  "Physical memory frame size mismatch");

    static inline bool isPageAligned(u64 address,
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