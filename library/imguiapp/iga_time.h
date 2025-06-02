#pragma once

#include <chrono>
#include <string>
#include <optional>
#include <fmt/format.h>
#include <fmt/chrono.h>

namespace iga::time {

enum class TimeDisplay {
    AbsDouble,
    DeltaDouble,
    AbsTime,
    // DeltaTime
};

enum class TimePrecision { Full, Millisecond, Decisecond, Second };


TimePrecision time_precision(TimePrecision* set = nullptr);
TimeDisplay time_display(TimeDisplay* set = nullptr);
bool include_date(bool* set = nullptr);


inline double toDouble(const std::chrono::system_clock::time_point& tp)
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(tp.time_since_epoch()).count();
}

inline double nowAsDouble()
{
    return toDouble(std::chrono::system_clock::now());
}


inline std::chrono::system_clock::time_point doubleAsTimePoint(double tUtc)
{
    return std::chrono::system_clock::time_point(
        std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<double>(tUtc)));
}


std::string toLocalStr(std::chrono::system_clock::time_point time,
                       TimePrecision precision = TimePrecision::Millisecond);

inline std::string toLocalStr(double time, TimePrecision precision = TimePrecision::Millisecond)
{
    return toLocalStr(doubleAsTimePoint(time), precision);
}

inline std::string toDateTimeString(const std::chrono::system_clock::time_point& tp)
{
    auto ms    = std::chrono::time_point_cast<std::chrono::milliseconds>(tp);
    auto local = std::chrono::current_zone()->to_local(ms);
    return std::format("{:%F %T}", local);
}

inline std::string toTimeString(const std::chrono::system_clock::time_point& tp)
{
    auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(tp);
    return std::format("{:%T}", std::chrono::current_zone()->to_local(ms));
}

inline std::string toDateTimeString(double tUTC)
{
    auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(doubleAsTimePoint(tUTC));
    return std::format("{:%F %T}", std::chrono::current_zone()->to_local(ms));
}

inline std::string toTimeString(double tUTC)
{
    auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(doubleAsTimePoint(tUTC));
    return std::format("{:%T}", std::chrono::current_zone()->to_local(ms));
}

std::string to_string(double tUTC, double now = 0.0);


inline double clock_seconds()
{
    static const auto start = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - start;
    return elapsed.count();
}


} // namespace iga::time
