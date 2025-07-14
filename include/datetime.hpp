#pragma once

#include <chrono>
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <iostream>
#include <cmath>
#include <tuple>

namespace TimeUtils {
    
// Clock epochs
constexpr auto GPS_EPOCH = std::chrono::sys_days{std::chrono::January/6/1980};
constexpr auto BDS_EPOCH = std::chrono::sys_days{std::chrono::January/1/2006};
constexpr auto UNIX_EPOCH = std::chrono::sys_days{std::chrono::January/1/1970};
constexpr auto JD_UNIX_EPOCH = 2440587.5; // Julian date of 1970-01-01 00:00:00 UTC

constexpr auto GPS_UTC_LEAP = std::chrono::seconds{18};
constexpr auto BDS_UTC_LEAP = std::chrono::seconds{4};
constexpr auto BDS_GPS_WEEKS = std::chrono::weeks{1356};
constexpr auto BDS_GPS_SEC = GPS_UTC_LEAP - BDS_UTC_LEAP;


class DateTime {
public:
    using time_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;
    time_point tp;

    DateTime(time_point tp_) : tp(tp_) {}
    DateTime(int year, int month, int day, int hour, int minute, int second,
             int millisecond = 0, int microsecond = 0, int nanosecond = 0);

    static DateTime from_gps_seconds(double gps_sec);

    static DateTime from_bds_seconds(double bds_sec) {
        return from_gps_seconds(bds_sec + 604800.0*BDS_GPS_WEEKS.count()+BDS_GPS_SEC.count());
    }

    static DateTime from_gps_week_sow(int week, double sow) {
        return from_gps_seconds(week*604800.0 + sow);
    }

    static DateTime from_bds_week_sow(int week, double sow) {
        return from_gps_week_sow(week + BDS_GPS_WEEKS.count(), sow + BDS_GPS_SEC.count());
    }

    static DateTime from_year_doy(int year, double doy_frac);

    static DateTime from_unix_timestamp(double unix_ts) {
        return DateTime(time_point{duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>{unix_ts})});
    }

    static DateTime from_julian_date(double jd) {
        using namespace std::chrono;

        double sec_since_unix = (jd - JD_UNIX_EPOCH) * 86400.0;

        auto dur = duration_cast<nanoseconds>(duration<double>{sec_since_unix});
        return DateTime(time_point{dur});
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

    auto get_ymd() const {
        return std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(tp));
    }
    auto get_tod() const {
        return tp - std::chrono::floor<std::chrono::days>(tp);
    }
    auto get_hms() const {
        return std::chrono::hh_mm_ss<std::chrono::nanoseconds>{get_tod()};
    }

    int year() const { return (int)get_ymd().year(); }
    int month() const { return (unsigned)get_ymd().month(); }
    int day() const { return (unsigned)get_ymd().day(); }
    int hour() const { return get_hms().hours().count(); }
    int minute() const { return get_hms().minutes().count(); }
    int second() const { return get_hms().seconds().count(); }
    int millisecond() const { return get_hms().subseconds().count() / 1000000; }
    int microsecond() const { return (get_hms().subseconds().count() / 1000) % 1000; }
    int nanosecond() const { return get_hms().subseconds().count() % 1000; }

    void set_year(int year) {
        tp = std::chrono::sys_days{std::chrono::year{year} / get_ymd().month() / get_ymd().day()} + get_tod();
    }
    void set_month(int month) {
        unsigned mon = static_cast<unsigned>(month);
        tp = std::chrono::sys_days{get_ymd().year() / std::chrono::month{mon} / get_ymd().day()} + get_tod();
    }
    void set_day(int day) {
        unsigned d = static_cast<unsigned>(day);
        tp = std::chrono::sys_days{get_ymd().year() / get_ymd().month() / std::chrono::day{d}} + get_tod();
    }
    void set_hour(int hour) {
        auto day = std::chrono::floor<std::chrono::days>(tp);
        tp = day + std::chrono::hours{hour} + std::chrono::minutes{get_hms().minutes()}
                + std::chrono::seconds{get_hms().seconds()} + get_hms().subseconds();
    }
    void set_minute(int minute) {
        auto day = std::chrono::floor<std::chrono::days>(tp);
        tp = day + std::chrono::hours{get_hms().hours()} + std::chrono::minutes{minute}
            + std::chrono::seconds{get_hms().seconds()} + get_hms().subseconds();
    }
    void set_second(int second) {
        auto day = std::chrono::floor<std::chrono::days>(tp);
        tp = day + std::chrono::hours{get_hms().hours()} + std::chrono::minutes{get_hms().minutes()}
            + std::chrono::seconds{second} + get_hms().subseconds();
    }

    void set_millisecond(int millisecond) {
        auto day = std::chrono::floor<std::chrono::days>(tp);
        auto tod = std::chrono::hh_mm_ss<std::chrono::nanoseconds>{tp - day};
        auto ns = std::chrono::nanoseconds{
            (tod.subseconds().count() % 1'000'000) + millisecond * 1'000'000
        };
        tp = day + std::chrono::hours{tod.hours()}
                + std::chrono::minutes{tod.minutes()}
                + std::chrono::seconds{tod.seconds()}
                + ns;
    }

    void set_microsecond(int microsecond) {
        auto day = std::chrono::floor<std::chrono::days>(tp);
        auto tod = std::chrono::hh_mm_ss<std::chrono::nanoseconds>{tp - day};
        auto ns = std::chrono::nanoseconds{
            (tod.subseconds().count() % 1'000) +
            (tod.subseconds().count() / 1'000'000 * 1'000'000) +  // keep ms
            microsecond * 1'000
        };
        tp = day + std::chrono::hours{tod.hours()}
                + std::chrono::minutes{tod.minutes()}
                + std::chrono::seconds{tod.seconds()}
                + ns;
    }

    void set_nanosecond(int nanosecond) {
        auto day = std::chrono::floor<std::chrono::days>(tp);
        auto tod = std::chrono::hh_mm_ss<std::chrono::nanoseconds>{tp - day};
        auto ns = std::chrono::nanoseconds{
            (tod.subseconds().count() / 1'000 * 1'000) + nanosecond
        };
        tp = day + std::chrono::hours{tod.hours()}
                + std::chrono::minutes{tod.minutes()}
                + std::chrono::seconds{tod.seconds()}
                + ns;
    }

    void add_year(int y) { set_year(year() + y); }
    void add_month(int m) { set_month(month() + m); }
    void add_day(int d) { set_day(day() + d); }
    void add_hour(int h) { set_hour(hour() + h); }
    void add_minute(int m) { set_minute(minute() + m); }
    void add_second(int s) { set_second(second() + s); }
    void add_millisecond(int ms) { set_millisecond(millisecond() + ms); }
    void add_microsecond(int us) { set_microsecond(microsecond() + us); }
    void add_nanosecond(int ns) { set_nanosecond(nanosecond() + ns); }

    
    std::pair<int, double> gps_week_sow() const {
        auto tot_sec = gps_seconds();
        int week = static_cast<int>(tot_sec / (7*86400));
        double sow = tot_sec - week*7*86400;

        return {week, sow};
    }

    double gps_seconds() const {
        using namespace std::chrono;

        auto duration = tp - time_point_cast<nanoseconds>(GPS_EPOCH);
        int64_t total_ns = duration_cast<nanoseconds>(duration).count();
        return static_cast<double>(total_ns) / 1000000000.0 + GPS_UTC_LEAP.count();
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

    std::pair<int, double> year_doy() const {
        using namespace std::chrono;

        auto ymd = get_ymd();
        auto y = ymd.year();

        int doy = duration_cast<days>(floor<days>(tp) - sys_days{y/1/1}).count() + 1;
        auto sec = duration_cast<duration<double>>(tp - floor<days>(tp)).count() / 86400.0;

        return {(int)y, doy + sec};

    }

    double julian_date() const {
        return JD_UNIX_EPOCH + duration_cast<std::chrono::duration<double>>(tp.time_since_epoch()).count() / 86400.0;
    }

    double unix_timestamp() const {
        return duration_cast<std::chrono::duration<double>>(tp.time_since_epoch()).count();
    }

    unsigned day_of_week() const {
        return std::chrono::weekday{floor<std::chrono::days>(tp)}.iso_encoding();
    }

    void print_all() const {
        fmt::println("========= DateTime Value =========");
        fmt::println("DateTime: {}-{}-{} {}:{}:{}.{},{},{}", year(), month(), day(), hour(), minute(), second(), millisecond(), microsecond(), nanosecond());
        fmt::println("GPS Week and Sec: {}", gps_week_sow());
        fmt::println("GPS Seconds: {}", gps_seconds());
        fmt::println("BDS Week and Sec: {}", bds_week_sow());
        fmt::println("BDS Seconds: {}", bds_seconds());
        fmt::println("Year and Doy: {}", year_doy());
        fmt::println("Julian Date: {}", julian_date());
        fmt::println("Unix Timestamp: {}", unix_timestamp());
        fmt::println("Day of Week: {}", day_of_week());
        fmt::println("==================================");
    }

private:
    // void adjust_timesys(time_point& tp, TSYS ts)
    // {
    //     switch (ts) {
    //         case TSYS::UTC:
    //             break;
    //         case TSYS::GPS:
    //             tp -= GPS_UTC_LEAP;
    //             break;
    //         case TSYS::BDS:
    //             tp -= BDS_UTC_LEAP;
    //             break;
    //     }
    // }

    static void split_seconds(double tot_sec, int& sec, int& ms, int& us, int& ns)
    {
        sec = static_cast<int>(tot_sec);
        double frac = tot_sec - sec;

        ms = static_cast<int>(frac * 1'000);
        frac -= ms / 1'000.0;

        us = static_cast<int>(frac * 1'000'000);
        frac -= us / 1'000'000.0;

        ns = static_cast<int>(frac * 1'000'000'000);
    }

    static double merge_seconds(const int sec, const int ms, const int us, const int ns)
    {
        return sec + (ms / 1000.0) + (us / 1000000.0) + (ns / 1000000000.0);
    }

};

}


template <>
struct fmt::formatter<TimeUtils::DateTime> : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(const TimeUtils::DateTime& d, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{}-{}-{} {}:{}:{}.{},{},{}", d.year(), d.month(), d.day(), 
            d.hour(), d.minute(), d.second(), d.millisecond(), d.microsecond(), d.nanosecond());
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