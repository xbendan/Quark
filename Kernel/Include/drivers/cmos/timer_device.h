#pragma once

#include <drivers/cmos/spec.h>
#include <mixins/utils/date.h>
#include <quark/dev/device.h>
#include <quark/hal/ports.h>
#include <quark/timing/clock_source.h>

namespace CMOS {
    using namespace Quark::System;

    class RealTimeClockDevice
        : public ClockSource
        , public Io::Device
    {
    public:
        RealTimeClockDevice()
            : ClockSource()
            , Device("Real Time Clock", Device::Type::TimerOrClock)
        {
        }

        Res<> OnInitialize() override final;

        bool IsUpdateInProgress();
        void ReadTime();
        u8   Get(int k);
        u8   GetSeconds();
        u8   GetMinutes();
        u8   GetHours();
        u8   GetDay();
        u8   GetMonth();
        u16  GetYear();

        virtual Date GetToday() override final;
        virtual u64  GetTimestamp() override final;

    private:
        Hal::PortAccess<CMOS_PORT_REG_INDEX> m_indexAccess;
        Hal::PortAccess<CMOS_PORT_REG_DATA>  m_dataAccess;
        u32                                  m_centuryRegsiter;

        u8 m_cen, m_year, m_month, m_day, m_hours, m_minutes, m_seconds;
    };
}