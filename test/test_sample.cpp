#include <chrono>
#include <fmt/core.h>

#include <gtest/gtest.h>

#include "date.hpp"
#include "time.hpp"
#include "datetime.hpp"
#include "datetime_operators.hpp"

using namespace TimeUtils;

Date d{2025, 2, 7};
Time t(11, 30, 45);


TEST(DateTime, construction)
{
    DateTime dt = d + t;

    dt.print_all();
}

TEST(DateTime, increment_time)
{
    DateTime dt = d + t;

    fmt::println("DateTime: {}", dt);

    ASSERT_EQ(dt.time().hour(), 11);
    ASSERT_EQ(dt.time().minute(), 30);
    ASSERT_EQ(dt.time().second(), 45);

    Time t{3, 35, 40};
    dt += t;

    fmt::println("DateTime: {}", dt);

    ASSERT_EQ(dt.time().hour(), 15);
    ASSERT_EQ(dt.time().minute(), 6);
    ASSERT_EQ(dt.time().second(), 25);

    Time t2{10, 56, 17};
    dt += t2;

    fmt::println("DateTime: {}", dt);
}

TEST(DateTime, GPS_SOW_Conversion)
{
    DateTime dt = d + t;

    auto [week, sow] = dt.gps_week_sow();
    DateTime dt2 = DateTime::from_gps_week_sow(week, sow);

    auto diff = (dt.tp - dt2.tp).count();
    fmt::println("GPS SOW Conversion Diff: {} sec", diff);

    ASSERT_LT(diff, 5e-6);
}

TEST(DateTime, GPS_Seconds_Conversion)
{
    DateTime dt = d + t;

    auto seconds = dt.gps_seconds();
    DateTime dt2 = DateTime::from_gps_seconds(seconds);

    auto diff = (dt.tp - dt2.tp).count();
    fmt::println("GPS Seconds Conversion Diff: {} sec", diff);

    ASSERT_LT(diff, 5e-6);
}

TEST(DateTime, BDS_SOW_Conversion)
{
    DateTime dt = d + t;

    auto [week, sow] = dt.bds_week_sow();
    DateTime dt2 = DateTime::from_bds_week_sow(week, sow);

    auto diff = (dt.tp - dt2.tp).count();
    fmt::println("BDS SOW Conversion Diff: {} sec", diff);

    ASSERT_LT(diff, 5e-6);
}

TEST(DateTime, BDS_Seconds_Conversion)
{
    DateTime dt = d + t;

    auto seconds = dt.bds_seconds();
    DateTime dt2 = DateTime::from_bds_seconds(seconds);

    auto diff = (dt.tp - dt2.tp).count();
    fmt::println("BDS Seconds Conversion Diff: {} sec", diff);

    ASSERT_LT(diff, 5e-6);
}

TEST(DateTime, Year_Doy_Conversion)
{
    DateTime dt = d + t;

    auto [year, doy] = dt.year_doy_fractional();
    DateTime dt2 = DateTime::from_year_doy(year, doy);

    auto diff = (dt.tp - dt2.tp).count();
    fmt::println("Year_Doy Conversion Diff: {} sec", diff);

    ASSERT_LT(diff, 5e-6);
}

TEST(DateTime, Unix_Timestamp_Conversion)
{
    DateTime dt = d + t;

    auto seconds = dt.unix_timestamp();
    DateTime dt2 = DateTime::from_unix_timestamp(seconds);

    auto diff = (dt.tp - dt2.tp).count();
    fmt::println("Unix Conversion Diff: {} sec", diff);

    ASSERT_LT(diff, 5e-6);
}


TEST(DateTime, Julian_Date_Conversion)
{
    DateTime dt = d + t;

    auto days = dt.julian_date();
    DateTime dt2 = DateTime::from_julian_date(days);
    auto days2 = dt2.julian_date();

    auto diff = (dt.tp - dt2.tp).count();
    fmt::println("Julian Conversion Diff: {} sec", diff);

    ASSERT_LT(diff, 2e-5);
}