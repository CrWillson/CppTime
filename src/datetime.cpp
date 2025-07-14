#include "datetime.hpp"

using namespace std::chrono;
using namespace TimeUtils;

DateTime::DateTime(int year, int month, int day, int hour, int minute, int second,
    int millisecond, int microsecond, int nanosecond)
{
    year_month_day ymd{std::chrono::year{year}, std::chrono::month{(unsigned)month}, std::chrono::day{(unsigned)day}};
    if (!ymd.ok()) return;

    auto days_since_epoch = sys_days(ymd);
    auto hms = hours{hour} + minutes{minute} + seconds{second}
                + milliseconds{millisecond}
                + microseconds{microsecond}
                + nanoseconds{nanosecond};

    sys_time<nanoseconds> full_time = days_since_epoch + hms;
    tp = time_point_cast<nanoseconds>(full_time);
}

DateTime DateTime::from_gps_seconds(double gps_sec) {
    auto [sec, ms, us, ns] = split_seconds(gps_sec);
    auto utc_time = GPS_EPOCH + seconds{sec} + milliseconds{ms} + microseconds{us} + nanoseconds{ns} - GPS_UTC_LEAP;
    return DateTime{time_point_cast<nanoseconds>(utc_time)};
}

DateTime DateTime::from_bds_seconds(double bds_sec) {
    return from_gps_seconds(bds_sec + 604800.0*BDS_GPS_WEEKS.count()+BDS_GPS_SEC.count());
}

DateTime DateTime::from_gps_week_sow(int week, double sow) {
    return from_gps_seconds(week*604800.0 + sow);
}

DateTime DateTime::from_bds_week_sow(int week, double sow) {
    return from_gps_week_sow(week + BDS_GPS_WEEKS.count(), sow + BDS_GPS_SEC.count());
}

DateTime DateTime::from_year_doy(int year, double doy_frac) {
    int day_int = static_cast<int>(doy_frac);
    double day_frac = doy_frac - day_int;

    auto base = sys_days{January/1/year} + days{day_int - 1};
    auto offset = duration_cast<nanoseconds>(duration<double>{day_frac * 86400.0});

    return DateTime(time_point_cast<nanoseconds>(base + offset));
}

DateTime DateTime::from_unix_timestamp(double unix_ts) {
    return DateTime(time_point_ns{duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>{unix_ts})});
}

DateTime DateTime::from_julian_date(double jd) {
   double sec_since_unix = (jd - JD_UNIX_EPOCH) * 86400.0;

    auto dur = duration_cast<nanoseconds>(duration<double>{sec_since_unix});
    return DateTime(time_point_ns{dur});
}

void DateTime::set_year(int year) {
    tp = std::chrono::sys_days{std::chrono::year{year} / get_ymd().month() / get_ymd().day()} + get_tod();
}
void DateTime::set_month(int month) {
    unsigned mon = static_cast<unsigned>(month);
    tp = std::chrono::sys_days{get_ymd().year() / std::chrono::month{mon} / get_ymd().day()} + get_tod();
}
void DateTime::set_day(int day) {
    unsigned d = static_cast<unsigned>(day);
    tp = std::chrono::sys_days{get_ymd().year() / get_ymd().month() / std::chrono::day{d}} + get_tod();
}
void DateTime::set_hour(int hour) {
    auto day = std::chrono::floor<std::chrono::days>(tp);
    tp = day + std::chrono::hours{hour} + std::chrono::minutes{get_hms().minutes()}
            + std::chrono::seconds{get_hms().seconds()} + get_hms().subseconds();
}
void DateTime::set_minute(int minute) {
    auto day = std::chrono::floor<std::chrono::days>(tp);
    tp = day + std::chrono::hours{get_hms().hours()} + std::chrono::minutes{minute}
        + std::chrono::seconds{get_hms().seconds()} + get_hms().subseconds();
}
void DateTime::set_second(int second) {
    auto day = std::chrono::floor<std::chrono::days>(tp);
    tp = day + std::chrono::hours{get_hms().hours()} + std::chrono::minutes{get_hms().minutes()}
        + std::chrono::seconds{second} + get_hms().subseconds();
}

void DateTime::set_millisecond(int millisecond) {
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

void DateTime::set_microsecond(int microsecond) {
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

void DateTime::set_nanosecond(int nanosecond) {
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