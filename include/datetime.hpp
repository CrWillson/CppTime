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

    struct DateTimeBreakdown {
        const int year;               // Years since 0 B.C. [yrs]
        const int month;              // The month [1 = Jan, 2 = Feb, ...]
        const int day;                // Day of the month [days]
        const int hour;               // Hour of the day [hr]
        const int minute;             // Minutes [min]
        const double second;          // Seconds and fraction of second [sec]
        const int gps_wn;             // GPS week number [wks]
        const double gps_sow;         // GPS seconds of week [sec]
        const double gps_seconds;     // GPS seconds [sec]
        const int doy;                // Day of the year [days]
        const double doy_fractional;  // Day of the year plus fraction of day [days]
        const double unix_timestamp;  // Unix timestamp [sec]
    };

    DateTime(const Date& date, const Time& time) : tp(std::chrono::sys_days{date.ymd} + time.sec) {}
    DateTime(std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> tp_) : tp(tp_) {}

    static DateTime from_gps_week_sow(int week, double sow) {
        return DateTime{GPS_EPOCH + std::chrono::weeks{week} + std::chrono::duration<double>(sow) - GPS_UTC_LEAP};
    }

    static DateTime from_bds_week_sow(int week, double sow) {
        return from_gps_week_sow(week + 1356, sow + 14);
    }

    static DateTime from_year_doy(int year, double doy_frac) {
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
        return from_gps_seconds(bds_sec + 604800*1356+14);
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

    std::tuple<int, int> year_doy() const {
        auto d = date();
        int doy = (std::chrono::sys_days{d.ymd} - std::chrono::sys_days{d.ymd.year()/1/1}).count() + 1;
        return {d.year(), doy};
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

    DateTimeBreakdown breakdown() const {
        const auto [wn, sow] = gps_week_sow();
        const auto [y, fdoy] = year_doy_fractional();
        const auto [y2, doy] = year_doy();

        return {
            date().year(), date().month(), date().day(), time().hour(),
            time().minute(), time().second(), wn, sow,
            gps_seconds(), doy, fdoy, unix_timestamp()
        };
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

namespace std {
    template <>
    struct hash<TimeUtils::DateTime> {
        std::size_t operator()(const TimeUtils::DateTime& dt) const {
            std::size_t h1 = std::hash<double>{}(dt.tp.time_since_epoch().count());

            // // Combine hashes manually
            // std::size_t combined = h1;
            // combined ^= h2 + 0x9e3779b9 + (combined << 6) + (combined >> 2);
            // combined ^= h3 + 0x9e3779b9 + (combined << 6) + (combined >> 2);

            return h1;
        }
    };
}