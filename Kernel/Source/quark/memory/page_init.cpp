#include <quark/memory/buddy_system.h>
#include <quark/memory/page.h>
#include <quark/memory/page_alloc.h>
#include <quark/os/main.h>

namespace Quark::System::Memory {
    extern BuddyZone buddyZones[ZONE_TYPES];
}

namespace Quark::System {
    using namespace Quark::System::Memory;

    Res<> InitPhysMemory()
    {
        auto& info          = BootInfo::MemoryInfo;
        auto& addressRanges = info._addressRanges;

        auto allocPhysBlock4K = [&info, &addressRanges](usize amount) -> u64 {
            Math::AlignUpRef(amount, PAGE_SIZE_4K);
            u64 address = 0;
            int i       = 0;
            while (addressRanges[i].Range.Length()) {
                if (addressRanges[i].Type != MemmapEntry::Free) {
                    i++;
                    continue;
                }

                Optional<u64> addr = addressRanges[i]              //
                                         .Range                    //
                                         .InnerAlign(PAGE_SIZE_4K) //
                                         .TakeFront(amount);
                if (addr.IsPresent()) {
                    address = addr.Take();
                    break;
                }
                i++;
            }
            if (info._addressSpace /* Determine if virtual paging is enabled */) {
                u64 virt = info._addressSpace
                               ->AllocateVirtPages4K(amount / PAGE_SIZE_4K)
                               .Unwrap();
                info._addressSpace->MapAddress4K(
                    address,
                    virt,
                    amount / PAGE_SIZE_4K,
                    (Hal::VmmFlags::WRITABLE | Hal::VmmFlags::PRESENT));
                address = virt;
            }
            return address;
        };

        usize memparts = DivCeil(info._limit, PAGE_SIZE_PARTITION);
        g_pageFrames   = reinterpret_cast<PageFrame**>(allocPhysBlock4K(
            Math::AlignUp(memparts * sizeof(PageFrame*), PAGE_SIZE_4K)));
        for (usize i = 0; i < memparts; i++) {
            g_pageFrames[i] = reinterpret_cast<PageFrame*>(
                allocPhysBlock4K(PAGE_BLOCK_AMOUNT * sizeof(PageFrame)));
        }

        // clang-format off
        new (&buddyZones[ZONE_DMA]) BuddyZone(0x0, 0xFFFFFF);           // DMA, 16 MiB
        new (&buddyZones[ZONE_DMA32]) BuddyZone(0x1000000, 0xFFFFFFFF); // DMA32, 4 GiB
        new (&buddyZones[ZONE_NORMAL]) BuddyZone(0x100000000, -1);      // Normal, Unlimited
        // clang-format on

        int num = ZONE_DMA;
        for (int i = 0;                                       //
             (i < addressRanges.Length())                     //
             && (addressRanges[i].Type != MemmapEntry::Unset) //
             && (num < ZONE_TYPES);
             i++) {

            if (addressRanges[i].Type != MemmapEntry::Free) {
                continue;
            }

            BuddyZone& zone = buddyZones[num];
            zone.MarkRegionAsFree(addressRanges[i].Range);
            if (addressRanges[i].Range.To() >= zone.GetRange().To()) {
                num++;
                i--;
            }
        }

        return Ok();
    }
}