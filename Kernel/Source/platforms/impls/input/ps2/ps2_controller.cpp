#include <drivers/acpi/device.h>
#include <drivers/ps2/device.h>
#include <quark/hal/ports.h>
#include <quark/os/diagnostic/logging.h>
#include <quark/sched/completable.h>

namespace PS2 {
    using namespace Quark::System::Hal;
    using namespace Quark::System::Diagnostic;

    LegacyControllerDevice::LegacyControllerDevice()
        : Io::Device("PS/2 Legacy Controller"s, Type::SystemDevices)
    {
    }

    Res<> LegacyControllerDevice::OnInitialize()
    {
        // Determine if PS2 Controller actually exists.
        /* PS2 Controller is not supported while the bit 1 of
           'IA PC Boot Architecture Flags' is clear (0). */
        ACPI::FixedAcpiDescTable* fadt;
        Device::FindByName<ACPI::ControllerDevice>(ACPI::ControllerDevice::Name)
            .IfPresent([&](ACPI::ControllerDevice* device) {
                fadt = device->FindTable<ACPI::FixedAcpiDescTable>("FACP");
            });
        if (fadt == nullptr) {
            return Error::DeviceFault("FADT table not found.");
        }

        // if (!(fadt->_flags & 0x2)) {
        //     error$("[PS/2] PS/2 Controller is not supported. {:#b}",
        //            fadt->_flags);
        //     return Error::DeviceFault("PS/2 Controller is not supported.");
        // }

        // Send(Command::DISABLE_FIRST_PORT);
        // Send(Command::DISABLE_SECOND_PORT);

        // /* Flush The Output Buffer */
        // Flush();

        // /* Set the Controller Configuration Byte */
        // Send(Command::READ_BYTE_ZERO);
        // u8 status =
        //     PortAccess<(usize)Command::READ_BYTE_ZERO>().In<u8>() &
        //     (u8)(~(ConfigByte::FirstPortClock | ConfigByte::SecondPortClock |
        //            ConfigByte::FirstPortTranslation) |
        //          ConfigByte::FirstPortInterrupt);
        // Send(Command::WRITE_BYTE_ZERO);
        // m_dataAccess << status;

        // /* Perform Controller Self-Test */
        // Send(Command::TEST_PS2_CONTROLLER);
        // Wait();
        // if (m_dataAccess.In<u8>() != 0x55) {
        //     return Error::DeviceFault("PS/2 Controller self-test failed.");
        // }

        /* Perform First Port Test */
        Send(Command::TEST_FIRST_PORT);
        Wait();
        if (m_dataAccess.In<u8>() == 0x00) {
            Send(Command::ENABLE_FIRST_PORT);

            // m_dataAccess << 0xF0;
            // m_dataAccess << 0x02;

            Device::Load(m_keyboard = new LegacyKeyboardDevice());
        }

        return Ok();
    }

    void LegacyControllerDevice::Send(PS2::Command command)
    {
        while (m_commandAccess.In<byte>() & STATE_INPUT_BUFFER)
            ;
        m_commandAccess << (byte)command;
    }

    void LegacyControllerDevice::Flush()
    {
        while (m_commandAccess.In<byte>() & STATE_OUTPUT_BUFFER)
            m_dataAccess.In<byte>();
    }

    void LegacyControllerDevice::Wait()
    {
        int timeout = 100;
        while (timeout-- && !(m_commandAccess.In<u8>() & STATE_OUTPUT_BUFFER))
            Task::Delay(1);
    }
}