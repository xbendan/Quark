#include <quark/hal/interrupts.h>

namespace Quark::System::Hal {
    InterruptsControllerDevice::InterruptsControllerDevice()
        : Device("Interrupts Controller Device", Device::Type::SystemDevices)
    {
    }

    Res<> InterruptsControllerDevice::AddHandler(int intno,
                                                 void (*handler)(Registers*))
    {
        if (intno >= 256) {
            return Error::IndexOutOfBounds("Invalid interrupt number");
        }

        m_interruptList[intno] += handler;
        return Ok();
    }

    Res<> InterruptsControllerDevice::RemoveHandler(int intno,
                                                    void (*handler)(Registers*))
    {
        if (intno >= 256) {
            return Error::IndexOutOfBounds("Invalid interrupt number");
        }

        m_interruptList[intno] -= handler;
        return Ok();
    }

    void InterruptsControllerDevice::CallEvent( //
        int        intno,
        Registers* frame)
    {
        if (intno >= m_interruptList.Count()) {
            return;
        }
    }
}