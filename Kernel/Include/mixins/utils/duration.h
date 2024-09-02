#include <mixins/std/c++types.h>
#include <mixins/std/time.h>

class Duration
{
public:
    Duration()                           = default;
    Duration(const Duration&)            = default;
    Duration(Duration&&)                 = default;
    Duration& operator=(const Duration&) = default;
    Duration& operator=(Duration&&)      = default;
    ~Duration()                          = default;

    Duration(u64 value, Std::TimeUnit unit)
        : m_value(value)
        , m_unit(unit)
    {
    }

    u64 value() const { return m_value; }

    Std::TimeUnit unit() const { return m_unit; }

    Duration operator+(const Duration& other) const
    {
        return Duration(m_value + other.m_value, m_unit);
    }

    Duration operator-(const Duration& other) const
    {
        return Duration(m_value - other.m_value, m_unit);
    }

    Duration operator*(u64 factor) const
    {
        return Duration(m_value * factor, m_unit);
    }

    Duration operator/(u64 factor) const
    {
        return Duration(m_value / factor, m_unit);
    }

    Duration operator%(u64 factor) const
    {
        return Duration(m_value % factor, m_unit);
    }

    bool operator==(const Duration& other) const
    {
        return m_value == other.m_value && m_unit == other.m_unit;
    }

    bool operator!=(const Duration& other) const
    {
        return m_value != other.m_value || m_unit != other.m_unit;
    }

    bool operator<(const Duration& other) const
    {
        return m_value < other.m_value ||
               (m_value == other.m_value && m_unit < other.m_unit);
    }

    bool operator<=(const Duration& other) const
    {
        return m_value < other.m_value ||
               (m_value == other.m_value && m_unit <= other.m_unit);
    }

    bool operator>(const Duration& other) const
    {
        return m_value > other.m_value ||
               (m_value == other.m_value && m_unit > other.m_unit);
    }

    bool operator>=(const Duration& other) const
    {
        return m_value > other.m_value ||
               (m_value == other.m_value && m_unit >= other.m_unit);
    }

private:
    u64           m_value;
    Std::TimeUnit m_unit;
};