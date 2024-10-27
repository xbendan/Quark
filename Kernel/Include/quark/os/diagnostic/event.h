#pragma once

#include <mixins/std/string.h>
#include <mixins/utils/date.h>

namespace Quark::System::Diagnostics {
    enum class RetentionPolicy
    {
        /*
         * Clear as every time system startup or shutdown
         * Keep on disk: No
         * When full:    Override
         * When partial: Append
         * When empty:   Append
         */
        NeverKeepOnDisk,
        /*
         * Overwrite the oldest event
         * Keep on disk: Yes
         * When full:    Override
         * When partial: Append
         * When empty:   Append
         */
        OverrideAsNeeded,
        /*
         * Overwrite the oldest event iff the new event is more critical
         * Keep on disk: Yes
         * When full:    Override iff new event is more critical
         * When partial: Append
         * When empty:   Append
         */
        OverrideAsNecessary,
        /*
         * Always keep the oldest event
         * Keep on disk: Yes
         * When full:    Do nothing
         * When partial: Append
         * When empty:   Append
         */
        AlwaysKeepOldest,
    };

    enum class EventType
    {
        Success,
        Info,
        Warning,
        Error,
        Critical,
    };

    struct EventSource
    {
        string          Name;
        RetentionPolicy Policy;
    };

    struct EventTrace
    {
        EventSource* Source;
        EventType    Type;
        Date         Time;
    };
}