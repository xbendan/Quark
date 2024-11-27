#include <drivers/acpi/timer.h>
#include <quark/hal/ports.h>
#include <quark/os/diagnostic/logging.h>

namespace ACPI {
    using namespace Quark::System::Diagnostic;
    using namespace Quark::System::Hal;

    ACPITimerDevice::ACPITimerDevice(ControllerDevice* controllerDevice)
        : Io::Device("ACPI Timer Device", Device::Type::TimerOrClock)
        , m_controllerDevice(controllerDevice)
        , m_ticks(0)
    {
        TimerSource::Global.PushBack(this);
    }

    Res<> ACPITimerDevice::OnInitialize()
    {
        assert(
            m_controllerDevice,
            "Cannot initialize ACPI timer device without a controller device.");
        ACPI::FixedAcpiDescTable* fadt = m_controllerDevice->TblFadt;
        assert(fadt, "ACPI requires a Fixed ACPI Description Table.");

        m_pmtTimerLength = fadt->_pmtTimerLength;
        m_xpmtTimerBlock = &fadt->x_pmtTimerBlock;

        m_ticks = fadt->_pmtTimerBlock;
        // if (m_xpmtTimerBlock->_address != 0) {
        //     switch (m_xpmtTimerBlock->_addressSpace) {
        //         case 0: {
        //             m_ticks = fadt->_pmtTimerBlock;
        //             break;
        //         }
        //         default: {
        //             warn$("Unsupported address space for ACPI
        //             timer block:
        //             {}",
        //                   m_xpmtTimerBlock->_addressSpace);
        //             break;
        //         }
        //     }
        // } else
        //     m_ticks = fadt->_pmtTimerBlock;

        m_useTimerExt = fadt->_flags & FADT_FLAGS_TMR_VAL_EXT;

        return Ok();
    }

    void ACPITimerDevice::Sleep(u64 ms)
    {
        u64 clk     = 3579545 * ms / 1000;
        u64 counter = 0;
        u64 last = GetSystemUptime(), curr = 0;
        while (counter < clk) {
            curr = GetSystemUptime();

            counter +=
                (curr < last)
                    ? ((m_useTimerExt ? 0x100000000 : 0x10000) - last + curr)
                    : curr - last;
            last = curr;
            asm volatile("nop");
        }
    }

    void ACPITimerDevice::SleepNanos(u64 ns)
    {
        Sleep(ns / 1000000);
    }

    Res<TimerAlarm> ACPITimerDevice::CreateAlarm(Date         date,
                                                 Func<void()> callback)
    {
        return Error::NotImplemented();
    }

    Res<TimerAlarm> ACPITimerDevice::CreateAlarm(TimeSpan     span,
                                                 Func<void()> callback)
    {
        return Error::NotImplemented();
    }

    u64 ACPITimerDevice::GetSystemUptime()
    {
        return in<u32>(m_ticks);
    }
}