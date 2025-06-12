#pragma once

#include <chrono>
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <iostream>
#include <cmath>

namespace TimeUtils {

class Date {
public:
    std::chrono::year_month_day ymd;

    Date(int y, unsigned m, unsigned d) : ymd{std::chrono::year{y}, std::chrono::month{m}, std::chrono::day{d}} {}
    Date(std::chrono::sys_days days) : ymd{std::chrono::year_month_day{days}} {}

    int year() const { return int(ymd.year()); }
    unsigned month() const { return unsigned(ymd.month()); }
    unsigned day() const { return unsigned(ymd.day()); }

    void set_year(int y) { ymd = std::chrono::year{y}/ymd.month()/ymd.day(); }
    void set_month(unsigned m) { ymd = ymd.year()/std::chrono::month{m}/ymd.day(); }
    void set_day(unsigned d) { ymd = ymd.year()/ymd.month()/std::chrono::day{d}; }

    operator std::chrono::sys_days() const { return std::chrono::sys_days{ymd}; }    

    friend bool operator==(const Date& a, const Date& b) { return std::chrono::sys_days{a} == std::chrono::sys_days{b}; }
    friend bool operator!=(const Date& a, const Date& b) { return !(a == b); }
    friend bool operator<(const Date& a, const Date& b) { return std::chrono::sys_days{a} < std::chrono::sys_days{b}; }
    friend bool operator<=(const Date& a, const Date& b) { return std::chrono::sys_days{a} <= std::chrono::sys_days{b}; }
    friend bool operator>(const Date& a, const Date& b) { return std::chrono::sys_days{a} > std::chrono::sys_days{b}; }
    friend bool operator>=(const Date& a, const Date& b) { return std::chrono::sys_days{a} >= std::chrono::sys_days{b}; }

    friend std::ostream& operator<<(std::ostream& os, const Date& d) {
        return os << fmt::format("{:04}-{:02}-{:02}", d.year(), d.month(), d.day());
    }

    friend fmt::formatter<Date>;
    friend class DateTime;
};

}

template <>
struct fmt::formatter<TimeUtils::Date> : fmt::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(const TimeUtils::Date& d, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{:04}-{:02}-{:02}", d.year(), d.month(), d.day());
    }
};
