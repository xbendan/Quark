#pragma once

#include <mixins/str/string.h>
#include <mixins/utils/date.h>
#include <quark/os/diagnostic/level.h>

namespace Quark::System::Diagnostic {
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

    struct EventSource
    {
        Qk::StringView  Name;
        RetentionPolicy Policy;
    };

    struct EventTrace
    {
        EventSource* Source;
        Level        Type;
        Date         Time;
    };
}