#include "time_utils.h"


namespace iga::time {

std::string toLocalStr(std::chrono::system_clock::time_point time, TimePrecision precision)
{
    using Decisecond  = std::chrono::duration<int64_t, std::deci>;
    using Millisecond = std::chrono::duration<int64_t, std::milli>;
    using Second      = std::chrono::duration<int64_t>;

    auto local = std::chrono::current_zone()->to_local(time);
    switch (precision) {
        default:
        case TimePrecision::Full: {
            return std::format("{:%H:%M:%S}", local);
        }
        case TimePrecision::Millisecond: {
            std::chrono::time_point tp = std::chrono::round<Millisecond>(local);
            return std::format("{:%H:%M:%S}", tp);
        }
        case TimePrecision::Decisecond: {
            std::chrono::time_point tp = std::chrono::round<Decisecond>(local);
            return std::format("{:%H:%M:%S}", tp);
        }
        case TimePrecision::Second: {
            std::chrono::time_point tp = std::chrono::round<Second>(local);
            return std::format("{:%H:%M:%S}", tp);
        }
    }
}

std::string to_string(double tUTC, double now)
{
    std::string output;
    switch (time_display()) {
        break;
        case TimeDisplay::AbsDouble: {
            switch (time_precision()) {
                break;
                case TimePrecision::Full: output = fmt::format("{}", tUTC); break;
                case TimePrecision::Millisecond: output = fmt::format("{:.3f}", tUTC); break;
                case TimePrecision::Decisecond: output = fmt::format("{:.1f}", tUTC); break;
                case TimePrecision::Second: output = fmt::format("{:.0f}", tUTC);
            }
        } break;
        case TimeDisplay::DeltaDouble: {
            now = (now != 0) ? now : nowAsDouble();
            switch (time_precision()) {
                break;
                case TimePrecision::Full: output = fmt::format("{}", now - tUTC); break;
                case TimePrecision::Millisecond: output = fmt::format("{:.3f}", now - tUTC); break;
                case TimePrecision::Decisecond: output = fmt::format("{:.1f}", now - tUTC); break;
                case TimePrecision::Second: output = fmt::format("{:.0f}", now - tUTC);
            }
        } break;
        case TimeDisplay::AbsTime: {
            output = toLocalStr(tUTC, time_precision());
        }
            // break; case TimeDisplay::DeltaTime: {
            //     now = (now != 0) ? now : Time::nowAsDouble();
            //     output = toLocalStr(now - tUTC, time_precision());
            // }
    }

    return output;
}

TimePrecision time_precision(TimePrecision* set)
{
    static TimePrecision precision = TimePrecision::Full;
    if (set) precision = *set;
    return precision;
}

TimeDisplay time_display(TimeDisplay* set)
{
    static TimeDisplay display = TimeDisplay::AbsDouble;
    if (set) display = *set;
    return display;
}

bool include_date(bool* set)
{
    static bool date = false;
    if (set) date = *set;
    return date;
}

} // namespace iga::time
