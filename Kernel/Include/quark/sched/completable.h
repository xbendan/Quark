#include <mixins/meta/event.h>
#include <mixins/meta/func.h>
#include <mixins/meta/tuple.h>
#include <mixins/std/c++types.h>
#include <mixins/std/time.h>
#include <mixins/utils/date.h>

namespace Quark::System::Task {
    using Std::TimeSpan;

    template <typename TResult = void>
    class CompletableTask
    {
    public:
        Event<TResult> OnComplete;
        void           Start();
        void           Cancel();
        bool           IsCompleted() const;

        template <typename TNewResult>
        CompletableTask<TNewResult> ContinueWith(
            Func<TNewResult(TResult)> action);

        template <typename TNewResult>
        CompletableTask<TNewResult> ContinueWith(Func<TNewResult()> action);

        CompletableTask<> Delay(u32 ms);
        CompletableTask<> Delay(TimeSpan span);
    };

    void Delay(u32 ms);
    void Delay(TimeSpan span);

    template <typename TResult = void>
    CompletableTask<TResult> Run(Func<TResult()> action);

    template <typename TResult = void>
    CompletableTask<TResult> Run(Func<TResult()> action, u32 delay);

    template <typename TResult>
    CompletableTask<TResult> Run(Func<TResult()> action, TimeSpan delay);

    template <typename TResult>
    CompletableTask<TResult> Run(Func<TResult()> action, Date when);

    template <typename TResult>
    CompletableTask<TResult> RunAsync(Func<TResult()> action);

    template <typename TResult>
    CompletableTask<TResult> RunAsync(Func<TResult()> action, u32 delay);

    template <typename TResult>
    CompletableTask<TResult> RunAsync(Func<TResult()> action, TimeSpan delay);

    template <typename TResult>
    CompletableTask<TResult> RunAsync(Func<TResult()> action, Date when);

    template <typename TResult>
    TResult Wait(CompletableTask<TResult> task);

    template <typename TResult>
    Opt<TResult> Wait(CompletableTask<TResult> task, u32 timeout);

    template <typename TResult>
    Opt<TResult> Wait(CompletableTask<TResult> task, TimeSpan timeout);

    template <typename... TResult>
    Tuple<TResult...> Wait(CompletableTask<TResult>... tasks);

    template <typename... TResult>
    Opt<Tuple<TResult...>> Wait(CompletableTask<TResult>... tasks, u32 timeout);

    template <typename... TResult>
    Opt<Tuple<TResult...>> Wait(CompletableTask<TResult>... tasks,
                                TimeSpan timeout);

    template <typename... TResult>
    Union<TResult...> WaitAny(CompletableTask<TResult>... tasks);

    template <typename... TResult>
    Opt<Union<TResult...>> WaitAny(CompletableTask<TResult>... tasks,
                                   u32 timeout);

    template <typename... TResult>
    Opt<Union<TResult...>> WaitAny(CompletableTask<TResult>... tasks,
                                   TimeSpan timeout);
}