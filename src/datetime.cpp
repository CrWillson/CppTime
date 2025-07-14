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
    int sec, ms, us, ns;
    split_seconds(gps_sec, sec, ms, us, ns);
    auto utc_time = GPS_EPOCH + seconds{sec} + milliseconds{ms} + microseconds{us} + nanoseconds{ns} - GPS_UTC_LEAP;
    return DateTime{time_point_cast<nanoseconds>(utc_time)};
}

DateTime DateTime::from_year_doy(int year, double doy_frac) {
    int day_int = static_cast<int>(doy_frac);
    double day_frac = doy_frac - day_int;

    auto base = sys_days{January/1/year} + days{day_int - 1};
    auto offset = duration_cast<nanoseconds>(duration<double>{day_frac * 86400.0});

    return DateTime(time_point_cast<nanoseconds>(base + offset));
}