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

        send(Command::DisableFirstPort);
        send(Command::DisableSecondPort);

        /* Flush The Output Buffer */
        flush();

        /* Set the Controller Configuration Byte */
        send(Command::ReadByteZero);
        u8 status =
            pIn<>((u8)Command::ReadByteZero) &
            (u8)(~(ConfigByte::FirstPortClock | ConfigByte::SecondPortClock |
                   ConfigByte::FirstPortTranslation) |
                 ConfigByte::FirstPortInterrupt);
        send(Command::WriteByteZero);
        pOut<>(PS2_DATA_PORT, status);

        /* Perform Controller Self-Test */
        send(Command::TestPs2Controller);
        wait();
        if (pIn<>(PS2_DATA_PORT) != 0x55) {
            return Error::DeviceFault("PS/2 Controller self-test failed.");
        }

        /* Perform First Port Test */
        send(Command::TestFirstPort);
        wait();
        if (pIn<>(PS2_DATA_PORT) == 0x00) {
            send(Command::EnableFirstPort);

            // pOut<>(PS2_DATA_PORT, 0xF0);
            // pOut<>(PS2_DATA_PORT, 0x02);

            Device::Load(m_keyboard = new LegacyKeyboardDevice());
        }

        return Ok();
    }

    void LegacyControllerDevice::send(PS2::Command command)
    {
        while (pIn<>(PS2_COMMAND_PORT) & (u8)PS2::StateReg::InputBuffer)
            ;
        pOut<>(PS2_COMMAND_PORT, (u8)command);
    }

    void LegacyControllerDevice::flush()
    {
        while (pIn<>(PS2_COMMAND_PORT) & (u8)StateReg::OutputBuffer)
            pIn<>(PS2_DATA_PORT);
    }

    void LegacyControllerDevice::wait()
    {
        int timeout = 100;
        while (timeout-- &&
               !(pIn<>(PS2_COMMAND_PORT) & (u8)StateReg::OutputBuffer))
            Task::Delay(1);
    }
}