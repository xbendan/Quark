#pragma once

#include <drivers/acpi/spec.h>
#include <drivers/apic/spec.h>
#include <quark/dev/device.h>
#include <quark/hal/hwobjs.h>
#include <quark/memory/address_space.h>

namespace APIC {
    using namespace Quark::System;
    using Quark::System::Hal::ProcessorDevice;

    class GenericControllerDevice : public Io::Device
    {
    public:
        class Local
        {
        public:
            Local(u8                       apicId,
                  GenericControllerDevice* apic,
                  ProcessorDevice*         processor)
                : m_apicId(apicId)
                , m_basePhys(apic->localBaseRead() & LOCAL_APIC_BASE)
                , m_baseVirt(Memory::copyAsIOAddress(m_basePhys))
                , m_cpu(processor)
            {
            }

            ~Local() = default;

            void regWrite(u32 reg, u32 data);
            u32  regRead(u32 reg);

            void sendIpi(u32 vec);

            void sendIpi(u32 dsh, u32 type, u8 vector)
            {
                sendIpi(dsh | type | ICR_VECTOR(vector));
            }

        private:
            u8               m_apicId;
            u64              m_basePhys;
            u64              m_baseVirt;
            ProcessorDevice* m_cpu;
        };

        GenericControllerDevice();
        ~GenericControllerDevice() = delete;

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