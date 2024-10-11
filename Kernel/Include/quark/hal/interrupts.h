#include <mixins/meta/event.h>
#include <mixins/meta/result.h>
#include <mixins/utils/linked_list.h>
#include <platforms/definition.h>
#include <quark/dev/device.h>

namespace Quark::System::Hal {
    using namespace Quark::System::Platform;
    using Quark::System::Io::Device;

    class InterruptsControllerDevice : public Device
    {
    public:
        InterruptsControllerDevice();

        Res<> AddHandler(int intno, void (*handler)(InterruptStackFrame*));
        Res<> RemoveHandler(int intno, void (*handler)(InterruptStackFrame*));

        void CallEvent(int intno, InterruptStackFrame* frame);

    private:
        LinkedList<Event<InterruptStackFrame*>> m_interruptList;
    };
}