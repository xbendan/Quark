#include <mixins/meta/ref.h>
#include <mixins/meta/result.h>
#include <mixins/std/c++types.h>
#include <mixins/std/string.h>

#include <quark/hal/task.h>
#include <quark/sched/process.h>
#include <quark/sched/sched.h>

namespace Quark::API {
    using Quark::Io::FileSystem::File;
    using Quark::Io::FileSystem::Folder;
    using Quark::Task::Process;
    using Quark::Task::Scheduler;
    using Quark::Task::Thread;

    /**
     * @brief Create a Process with specified name
     * The process will not be started until it has a valid entry point
     * with a existed file.
     *
     * @param name
     * @return Process* pointer to the created process
     */
    Res<RefPtr<Process>> createProcess(String<Utf8> name);
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
        String<Utf8> name,
        File*        file,
        Folder*      workingDirectory,
        Args&&... launchArgs);
    /**
     * @brief Create a thread with specified parent process
     *
     * @param process
     * @return Thread*
     */
    Res<Thread*>         createThread(Process* process);
    Res<Thread*>         createThreadEx(Process*        process,
                                        u8              priority,
                                        const String<>& name,
                                        const String<>& description,
                                        const String<>& command,
                                        const String<>& arguments);
    Scheduler*           getScheduler();
}

#if defined(GLOBAL_API_NAMESPACE)
using namespace Quark::API;
#endif