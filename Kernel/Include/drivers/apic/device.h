#pragma once

#include <drivers/acpi/spec.h>
#include <drivers/apic/spec.h>
#include <quark/dev/device.h>
#include <quark/hal/hwobjs.h>
#include <quark/hal/task.h>
#include <quark/memory/address_space.h>

namespace APIC {
    using namespace Quark::System;
    using Quark::System::Hal::ProcessorDevice;

    class GenericControllerDevice : public Io::Device
    {
    public:
        struct Local
        {
            Local(u8                       apicId,
                  GenericControllerDevice* apic,
                  ProcessorDevice*         processor)
                : _apicId(apicId)
                , _basePhys(apic->localBaseRead() & LOCAL_APIC_BASE)
                , _baseVirt(Memory::copyAsIOAddress(_basePhys))
                , _cpu(processor)
            {
            }

            ~Local() = default;

            void regWrite(u32 reg, u32 data);
            u32  regRead(u32 reg);

            void setEnabled();
            void callIPI(u32 vec);
            void callIPI(u32 dsh, u32 type, u8 vector)
            {
                callIPI(dsh | type | ICR_VECTOR(vector));
            }

            u8               _apicId;
            u64              _basePhys;
            u64              _baseVirt;
            ProcessorDevice* _cpu;
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

        LinkedList<Local*>* getApicLocals() const { return m_units; }
        Local*              getApicLocal(u8 id);

        Res<> onLoad() override;

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