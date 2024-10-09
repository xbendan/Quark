#pragma once

#include <drivers/acpi/spec.h>
#include <drivers/apic/spec.h>
#include <quark/dev/device.h>
#include <quark/hal/multiprocessing.h>
#include <quark/memory/address_space.h>

namespace APIC {
    using namespace Quark::System;
    using namespace Quark::System::Hal;

    class GenericControllerDevice : public Io::Device
    {
    public:
        struct Local
        {
            Local(u8                       apicId,
                  GenericControllerDevice* apic,
                  ICPULocalDevice*         device)
                : _apicId(apicId)
                , _basePhys(apic->localBaseRead() & LOCAL_APIC_BASE)
                , _baseVirt(Memory::copyAsIOAddress(_basePhys))
                , _device(device)
            {
            }

            ~Local() = default;

            void RegWrite(u32 reg, u32 data);
            u32  RegRead(u32 reg);

            void Enable();
            void CallIPI(u32 vec);
            void CallIPI(u32 dsh, u32 type, u8 vector);

            u8               _apicId;
            u64              _basePhys;
            u64              _baseVirt;
            ICPULocalDevice* _device;
        };

        GenericControllerDevice();
        ~GenericControllerDevice() = default;

        /* --- Methods --- */
        void ioRegWrite32(u32 reg, u32 data);
        u32  ioRegRead32(u32 reg);
        void ioRegWrite64(u32 reg, u64 data);
        u64  ioRegRead64(u32 reg);
        void ioRedTblWrite(u32 index, u64 data);
        u64  ioRedTblRead(u32 index);

        void localBaseWrite(u64 data);
        u64  localBaseRead();
        void localRegWrite(u32 reg, u32 data);
        u32  localRegRead(u32 reg);

        LinkedList<Local*>* GetAPICLocals() const { return m_units; }
        Local*              GetAPICLocal(u8 id) { return (*m_units)[id]; }

        Res<> OnInitialize() override;

    private:
        u64           m_ioBasePhys;
        u64           m_ioBaseVirt;
        u32           m_interrupts;
        volatile u32* m_ioRegSel;
        volatile u32* m_ioWindow;

        LinkedList<Local*>* m_units;
        LinkedList<ACPI::MultiApicDescTable::InterruptServiceOverride*>*
            m_overrides;
    };
} // namespace Quark::System::Hal