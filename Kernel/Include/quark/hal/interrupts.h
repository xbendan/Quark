#pragma once

#include <mixins/meta/event.h>
#include <mixins/meta/result.h>
#include <mixins/utils/linked_list.h>
#include <quark/dev/device.h>
#include <quark/hal/definition.h>

#if defined(__x86_64__)
#include <platforms/x86_64/hwinterrupts.h>
#elif defined(__aarch64__)

#elif defined(__risv__)

#endif

namespace Quark::System::Hal {
    using Quark::System::Io::Device;
    using Registers = Platform::__ARCH_NAMESPACE::Registers;

    class InterruptsControllerDevice : public Device
    {
    public:
        InterruptsControllerDevice();

        Res<> AddHandler(int intno, void (*handler)(Registers*));
        Res<> RemoveHandler(int intno, void (*handler)(Registers*));

        void CallEvent(int intno, Registers* frame);

    private:
        LinkedList<Event<Registers*>> m_interruptList;
    };
}