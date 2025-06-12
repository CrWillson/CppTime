#pragma once

#include <chrono>
#include <string>
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <iostream>
#include <cmath>
#include <tuple>

#include "time.hpp"
#include "date.hpp"

namespace TimeUtils {
    
// Clock epochs
constexpr auto GPS_EPOCH = std::chrono::sys_days{std::chrono::year{1980}/1/6};
constexpr auto BDS_EPOCH = std::chrono::sys_days{std::chrono::year{2006}/1/1};
constexpr auto UNIX_EPOCH = std::chrono::sys_days{std::chrono::year{1970}/1/1};
constexpr auto JD_UNIX_EPOCH = 2440587.5; // Julian date of 1970-01-01 00:00:00 UTC

constexpr std::chrono::seconds GPS_UTC_LEAP = std::chrono::seconds{18};
constexpr std::chrono::seconds BDS_GPS_OFFSET = std::chrono::seconds{14};

class DateTime {
public:
    std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> tp;

    DateTime(const Date& date, const Time& time) : tp(std::chrono::sys_days{date.ymd} + time.sec) {}
    DateTime(std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> tp_) : tp(tp_) {}

    static DateTime from_gps_week_sow(int week, double sow) {
        return DateTime{GPS_EPOCH + std::chrono::weeks{week} + std::chrono::duration<double>(sow) - GPS_UTC_LEAP};
    }

    static DateTime from_bds_week_sow(int week, double sow) {
        return DateTime{BDS_EPOCH + std::chrono::weeks{week} + std::chrono::duration<double>(sow) - BDS_GPS_OFFSET - GPS_UTC_LEAP};
    }

    static DateTime from_year_doy_fractional(int year, double doy_frac) {
        auto base = std::chrono::sys_days{std::chrono::year{year}/1/1};
        int doy = static_cast<int>(floor(doy_frac));
        double frac_day = doy_frac - doy;
        return DateTime{base + std::chrono::days{doy - 1} + std::chrono::duration<double>(frac_day * 86400.0)};
    }

    static DateTime from_unix_timestamp(double timestamp) {
        return DateTime{UNIX_EPOCH + std::chrono::duration<double>(timestamp)};
    }

    static DateTime from_julian_date(double jd) {
        return DateTime{UNIX_EPOCH + std::chrono::duration<double>((jd - JD_UNIX_EPOCH) * 86400.0)};
    }

    static DateTime from_gps_seconds(double gps_sec) {
        return DateTime{GPS_EPOCH + std::chrono::duration<double>(gps_sec) - GPS_UTC_LEAP};
    }

    static DateTime from_bds_seconds(double bds_sec) {
        return DateTime{BDS_EPOCH + std::chrono::duration<double>(bds_sec) - BDS_GPS_OFFSET - GPS_UTC_LEAP};
    }

    friend bool operator==(const DateTime& a, const DateTime& b) { return a.tp == b.tp; }
    friend bool operator!=(const DateTime& a, const DateTime& b) { return !(a == b); }
    friend bool operator<(const DateTime& a, const DateTime& b) { return a.tp < b.tp; }
    friend bool operator<=(const DateTime& a, const DateTime& b) { return a.tp <= b.tp; }
    friend bool operator>(const DateTime& a, const DateTime& b) { return a.tp > b.tp; }
    friend bool operator>=(const DateTime& a, const DateTime& b) { return a.tp >= b.tp; }

    friend std::ostream& operator<<(std::ostream& os, const DateTime& dt) {
        return os << fmt::format("{:%F %T}", floor<std::chrono::seconds>(dt.tp))
                  << fmt::format(".{:03.0f}", fmod(dt.tp.time_since_epoch().count(), 1.0) * 1000);
    }

    friend fmt::formatter<DateTime>;

    Date date() const { return Date{floor<std::chrono::days>(tp)}; }
    Time time() const { return Time{std::chrono::duration<double>(tp - floor<std::chrono::days>(tp))}; }

    void set_year(int y) { auto d = date(); d.set_year(y); tp = std::chrono::sys_days{d.ymd} + time().sec; }
    void set_month(unsigned m) { auto d = date(); d.set_month(m); tp = std::chrono::sys_days{d.ymd} + time().sec; }
    void set_day(unsigned d_) { auto d = date(); d.set_day(d_); tp = std::chrono::sys_days{d.ymd} + time().sec; }
    void set_hour(int h) { auto t = time(); t.set_hour(h); tp = std::chrono::sys_days{date().ymd} + t.sec; }
    void set_minute(int m) { auto t = time(); t.set_minute(m); tp = std::chrono::sys_days{date().ymd} + t.sec; }
    void set_second(double s) { auto t = time(); t.set_second(s); tp = std::chrono::sys_days{date().ymd} + t.sec; }

    std::tuple<int, double> gps_week_sow() const {
        auto gps_tp = tp + GPS_UTC_LEAP;
        auto since_epoch = gps_tp - GPS_EPOCH;
        int week = std::chrono::duration_cast<std::chrono::weeks>(since_epoch).count();
        double sow = std::chrono::duration_cast<std::chrono::duration<double>>(since_epoch - std::chrono::weeks{week}).count();
        return {week, sow};
    }

    double gps_seconds() const {
        auto [weeks, sow] = gps_week_sow();
        return 604800*weeks + sow;
    }

    std::tuple<int, double> bds_week_sow() const {
        auto [weeks, sow] = gps_week_sow();
        weeks -= 1356;
        sow -= 14;
        return {weeks, sow};
    }

    double bds_seconds() const {
        auto [weeks, sow] = bds_week_sow();
        return 604800*weeks + sow;
    }

    std::tuple<int, double> year_doy_fractional() const {
        auto d = date();
        int doy = (std::chrono::sys_days{d.ymd} - std::chrono::sys_days{d.ymd.year()/1/1}).count() + 1;
        double frac_day = time().sec.count() / 86400.0;
        return {d.year(), doy + frac_day};
    }

    double julian_date() const {
        return JD_UNIX_EPOCH + duration_cast<std::chrono::duration<double>>(tp - UNIX_EPOCH).count() / 86400.0;
    }

    double unix_timestamp() const {
        return duration_cast<std::chrono::duration<double>>(tp - UNIX_EPOCH).count();
    }

    unsigned week_of_year() const {
        auto d = date();
        auto first_day = std::chrono::sys_days{d.ymd.year()/1/1};
        auto weekday_first = std::chrono::weekday{first_day}.c_encoding();
        auto weekday_current = std::chrono::weekday{std::chrono::sys_days{d.ymd}}.c_encoding();
        auto doy = (std::chrono::sys_days{d.ymd} - first_day).count();
        return (doy + weekday_first) / 7 + 1;
    }

    unsigned day_of_week() const {
        return std::chrono::weekday{floor<std::chrono::days>(tp)}.iso_encoding();
    }

    void print_all() const {
        fmt::println("========= DateTime Value =========");
        fmt::println("DateTime: {} {}", date(), time());
        fmt::println("GPS Week and Sec: {}", gps_week_sow());
        fmt::println("GPS Seconds: {}", gps_seconds());
        fmt::println("BDS Week and Sec: {}", bds_week_sow());
        fmt::println("BDS Seconds: {}", bds_seconds());
        fmt::println("Year and Doy: {}", year_doy_fractional());
        fmt::println("Julian Date: {}", julian_date());
        fmt::println("Unix Timestamp: {}", unix_timestamp());
        fmt::println("Week of Year: {}", week_of_year());
        fmt::println("Day of Week: {}", day_of_week());
        fmt::println("==================================");
    }
};

}


template <>
struct fmt::formatter<TimeUtils::DateTime> : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(const TimeUtils::DateTime& d, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{} {}", d.date(), d.time());
    }
};
