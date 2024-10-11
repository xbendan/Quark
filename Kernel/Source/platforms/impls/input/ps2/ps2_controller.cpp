#include <drivers/acpi/device.h>
#include <drivers/ps2/device.h>
#include <quark/hal/ports.h>
#include <quark/sched/completable.h>

namespace PS2 {
    using namespace Quark::System::Hal;

    LegacyControllerDevice::LegacyControllerDevice()
        : Io::Device("PS/2 Legacy Controller")
    {
    }

    Res<> LegacyControllerDevice::OnInitialize()
    {
        // Determine if PS2 Controller actually exists.
        /* PS2 Controller is not supported while the bit 1 of
           'IA PC Boot Architecture Flags' is clear (0). */
        ACPI::FixedAcpiDescTable* fadt;
        Device::FindByName<ACPI::ControllerDevice>(
            "Advanced Configuration & Power Interface Controller")
            .IfPresent([&](ACPI::ControllerDevice* device) {
                fadt = device->FindTable<ACPI::FixedAcpiDescTable>("FACP")
                           .Unwrap();
            });
        if (fadt == nullptr) {
            return Error::DeviceFault("FADT table not found.");
        }

        Send(Command::DisableFirstPort);
        Send(Command::DisableSecondPort);

        /* Flush The Output Buffer */
        Flush();

        /* Set the Controller Configuration Byte */
        Send(Command::ReadByteZero);
        u8 status =
            PortAccess<(usize)Command::ReadByteZero>().In<u8>() &
            (u8)(~(ConfigByte::FirstPortClock | ConfigByte::SecondPortClock |
                   ConfigByte::FirstPortTranslation) |
                 ConfigByte::FirstPortInterrupt);
        Send(Command::WriteByteZero);
        m_dataAccess << status;

        /* Perform Controller Self-Test */
        Send(Command::TestPs2Controller);
        Wait();
        if (m_dataAccess.In<u8>() != 0x55) {
            return Error::DeviceFault("PS/2 Controller self-test failed.");
        }

        /* Perform First Port Test */
        Send(Command::TestFirstPort);
        Wait();
        if (m_dataAccess.In<u8>() == 0x00) {
            Send(Command::EnableFirstPort);

            // pOut<>(PS2_DATA_PORT, 0xF0);
            // pOut<>(PS2_DATA_PORT, 0x02);

            Device::Load(m_keyboard = new LegacyKeyboardDevice());
        }

        return Ok();
    }

    void LegacyControllerDevice::Send(PS2::Command command)
    {
        while (m_commandAccess.In<u8>() & (u8)PS2::StateReg::InputBuffer)
            ;
        m_commandAccess << (u8)command;
    }

    void LegacyControllerDevice::Flush()
    {
        while (m_commandAccess.In<u8>() & (u8)StateReg::OutputBuffer)
            m_dataAccess.In<u8>();
    }

    void LegacyControllerDevice::Wait()
    {
        int timeout = 100;
        while (timeout-- &&
               !(m_commandAccess.In<u8>() & (u8)StateReg::OutputBuffer))
            Task::Delay(1);
    }
}