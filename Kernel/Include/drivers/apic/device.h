#pragma once

#include <drivers/acpi/spec.h>
#include <drivers/apic/spec.h>
#include <mixins/utils/list.h>
#include <quark/dev/device.h>
#include <quark/hal/multiprocessing.h>
#include <quark/memory/address_space.h>

namespace APIC {
    using namespace Quark::System;
    using namespace Quark::System::Hal;
    using Qk::List;

    class GenericControllerDevice : public Io::Device
    {
    public:
        struct Local
        {
            Local(u8                       processorId,
                  u8                       apicId,
                  GenericControllerDevice* apic,
                  ICPULocalDevice*         device)
                : _processorId(processorId)
                , _apicId(apicId)
                , _apic(apic)
                , _basePhys(apic->ReadBase() & LOCAL_APIC_BASE)
                , _baseVirt(Memory::CopyAsIOAddress(_basePhys))
                , _device(device)
            {
            }

            ~Local() = default;

            void RegWrite(u32 reg, u32 data);
            u32  RegRead(u32 reg);

            void Enable();
            void SendIPI(u32 vec);
            void SendIPI(u32 dsh, u32 type, u8 vector);

            u8                       _processorId;
            u8                       _apicId;
            GenericControllerDevice* _apic;
            u64                      _basePhys;
            u64                      _baseVirt;
            ICPULocalDevice*         _device;
        };

        GenericControllerDevice();
        ~GenericControllerDevice() = default;

        /* --- Methods --- */
        void WriteReg32(u32 reg, u32 data);
        u32  ReadReg32(u32 reg);
        void WriteReg64(u32 reg, u64 data);
        u64  ReadReg64(u32 reg);
        void WriteRedTbl(u32 index, u64 data);
        u64  ReadRedTbl(u32 index);

        void WriteBase(u64 data);
        u64  ReadBase();
        void WriteRegLoc(u32 reg, u32 data);
        u32  ReadRegLoc(u32 reg);

        List<Local*>& GetApicLocals() { return m_units; }
        Local*        GetApicLocal(u8 id) { return (m_units)[id]; }
        List<ACPI::MADT::InterruptServiceOverride*>& GetInterruptOverrides()
        {
            return m_overrides;
        }

        void Redirect(u8 irq, u8 vector, u32 delivery);
        void SendIPI(u8 dest, u32 dsh, u32 type, u8 vector);

        Res<> OnInitialize() override;

        static inline const char* Name =
            "Advanced Programmable Interrupt Controller";

    private:
        u64           m_locBasePhys;
        u64           m_locBaseVirt;
        u64           m_ioBasePhys;
        u64           m_ioBaseVirt;
        u32           m_interrupts;
        volatile u32* m_ioRegSel;
        volatile u32* m_ioWindow;

        List<Local*>                                m_units;
        List<ACPI::MADT::InterruptServiceOverride*> m_overrides;
    };
} // namespace Quark::System::Hal