#pragma once

#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <chrono>
#include <iostream>
#include <cmath>

namespace TimeUtils {

class Time {
public:
    std::chrono::duration<double> sec;

    Time(int h, int m, double s) : sec(std::chrono::hours(h) + std::chrono::minutes(m) + std::chrono::duration<double>(s)) {}
    Time(std::chrono::duration<double> s) : sec(s) {}

    int hour() const { return static_cast<int>(std::chrono::duration_cast<std::chrono::hours>(sec).count()); }
    int minute() const { return static_cast<int>(std::chrono::duration_cast<std::chrono::minutes>(sec - std::chrono::hours(hour())).count()); }
    double second() const { return (sec - std::chrono::hours(hour()) - std::chrono::minutes(minute())).count(); }

    void set_hour(int h) { sec = std::chrono::duration<double>(h * 3600 + minute() * 60 + second()); }
    void set_minute(int m) { sec = std::chrono::duration<double>(hour() * 3600 + m * 60 + second()); }
    void set_second(double s) { sec = std::chrono::duration<double>(hour() * 3600 + minute() * 60 + s); }

    friend bool operator==(const Time& a, const Time& b) { return a.sec == b.sec; }
    friend bool operator!=(const Time& a, const Time& b) { return !(a == b); }
    friend bool operator<(const Time& a, const Time& b) { return a.sec < b.sec; }
    friend bool operator<=(const Time& a, const Time& b) { return a.sec <= b.sec; }
    friend bool operator>(const Time& a, const Time& b) { return a.sec > b.sec; }
    friend bool operator>=(const Time& a, const Time& b) { return a.sec >= b.sec; }

    friend std::ostream& operator<<(std::ostream& os, const Time& t) {
        return os << fmt::format("{:02}:{:02}:{:06.3f}", t.hour(), t.minute(), t.second());
    }

    friend fmt::formatter<Time>;
};

}

template <>
struct fmt::formatter<TimeUtils::Time> : fmt::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(const TimeUtils::Time& t, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{:02}:{:02}:{:06.3f}", t.hour(), t.minute(), t.second());
    }
};