#include <mixins/meta/ref.h>
#include <mixins/meta/result.h>
#include <mixins/std/c++types.h>
#include <mixins/std/string.h>

#include <quark/hal/multiprocessing.h>
#include <quark/sched/process.h>
#include <quark/sched/sched.h>

namespace Quark::System::API {
    using Quark::System::Hal::ICPULocalDevice;
    using Quark::System::Io::FileSystem::File;
    using Quark::System::Io::FileSystem::Folder;
    using Quark::System::Task::Process;
    using Quark::System::Task::Thread;

    /**
     * @brief Create a Process with specified name
     * The process will not be started until it has a valid entry point
     * with a existed file.
     *
     * @param name
     * @return Process* pointer to the created process
     */
    Res<RefPtr<Process>> createProcess(string name);
    /**
     * @brief Create an idle process
     * The idle process will be started immediately after it is created,
     * but consume no resources.
     *
     * @return Process*
     */
    Res<RefPtr<Process>> createIdleProcess();
    /**
     * @brief Create a process with full arguments
     *
     * @param name
     * @return Process*
     */
    template <typename... Args>
    Res<RefPtr<Process>> createProcessEx( //
        string  name,
        File*   file,
        Folder* workingDirectory,
        Args&&... launchArgs);
    /**
     * @brief Create a thread with specified parent process
     *
     * @param process
     * @return Thread*
     */
    Res<Thread*>         createThread(Process* process);
    Res<Thread*>         createIdleThread();
    Res<Thread*>         createThreadEx(Process*      process,
                                        u8            priority,
                                        const string& name,
                                        const string& description,
                                        const string& command,
                                        const string& arguments);

    Res<ICPULocalDevice*> getCPULocal(u32 id);
    Res<>                 setCPULocal(u32 id, ICPULocalDevice* local);
}

#if defined(GLOBAL_API_NAMESPACE)
using namespace Quark::System::API;
#endif