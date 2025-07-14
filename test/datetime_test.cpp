#include <fmt/core.h>

#include <gtest/gtest.h>
#include "datetime.hpp"


using namespace TimeUtils;

TEST(DateTime, increment_time)
{
    DateTime dt{2025, 2, 7, 11, 30, 45, 123, 456, 789};

    fmt::println("DateTime: {}", dt);

    ASSERT_EQ(dt.year(), 2025);
    ASSERT_EQ(dt.month(), 2);
    ASSERT_EQ(dt.day(), 7);
    ASSERT_EQ(dt.hour(), 11);
    ASSERT_EQ(dt.minute(), 30);
    ASSERT_EQ(dt.second(), 45);
    ASSERT_EQ(dt.millisecond(), 123);
    ASSERT_EQ(dt.microsecond(), 456);
    ASSERT_EQ(dt.nanosecond(), 789);

    dt.add_day(1);
    dt.add_minute(45);
    dt.add_second(10);
    dt.add_microsecond(123);

    fmt::println("DateTime: {}", dt);

    ASSERT_EQ(dt.year(), 2025);
    ASSERT_EQ(dt.month(), 2);
    ASSERT_EQ(dt.day(), 8);
    ASSERT_EQ(dt.hour(), 12);
    ASSERT_EQ(dt.minute(), 15);
    ASSERT_EQ(dt.second(), 55);
    ASSERT_EQ(dt.millisecond(), 123);
    ASSERT_EQ(dt.microsecond(), 579);
    ASSERT_EQ(dt.nanosecond(), 789);
}

TEST(DateTime, Year_Doy_to_GPS)
{
    const int YEAR = 2025;
    const double DOY = 195.75;

    DateTime dt = DateTime::from_year_doy(YEAR, DOY);

    auto [week, sow] = dt.gps_week_sow();

    ASSERT_EQ(week, 2375);
    ASSERT_EQ(sow, 151218);

    DateTime dt2 = DateTime::from_gps_week_sow(week, sow);

    auto [year, doy] = dt2.year_doy();

    ASSERT_EQ(year, YEAR);
    ASSERT_NEAR(doy, DOY, 1e-6);
}

TEST(DateTime, GPS_SOW_Conversion)
{
    DateTime dt{2025, 2, 7, 11, 30, 45};

    auto [week, sow] = dt.gps_week_sow();

    ASSERT_EQ(week, 2352);
    ASSERT_NEAR(sow, 473463, 1e-6);

    DateTime dt2 = DateTime::from_gps_week_sow(week, sow);

    auto diff = (dt.tp - dt2.tp).count();
    fmt::println("GPS SOW Conversion Diff: {} ns", diff);

    ASSERT_LE(diff, 1);
}

TEST(DateTime, GPS_Seconds_Conversion)
{
    DateTime dt{2025, 2, 7, 11, 30, 45};

    auto seconds = dt.gps_seconds();
    DateTime dt2 = DateTime::from_gps_seconds(seconds);

    auto diff = (dt.tp - dt2.tp).count();
    fmt::println("GPS Seconds Conversion Diff: {} ns", diff);

    ASSERT_LE(diff, 1);
}

TEST(DateTime, BDS_SOW_Conversion)
{
    DateTime dt{2025, 2, 7, 11, 30, 45};

    auto [week, sow] = dt.bds_week_sow();
    DateTime dt2 = DateTime::from_bds_week_sow(week, sow);

    auto diff = (dt.tp - dt2.tp).count();
    fmt::println("BDS SOW Conversion Diff: {} ns", diff);

    ASSERT_LE(diff, 1);
}

TEST(DateTime, BDS_Seconds_Conversion)
{
    DateTime dt{2025, 2, 7, 11, 30, 45};

    auto seconds = dt.bds_seconds();
    DateTime dt2 = DateTime::from_bds_seconds(seconds);

    auto diff = (dt.tp - dt2.tp).count();
    fmt::println("BDS Seconds Conversion Diff: {} ns", diff);

    ASSERT_LE(diff, 1);
}

TEST(DateTime, Year_Doy_Conversion)
{
    DateTime dt{2025, 2, 7, 11, 30, 45};

    auto [year, doy] = dt.year_doy();
    DateTime dt2 = DateTime::from_year_doy(year, doy);

    auto diff = (dt.tp - dt2.tp).count();
    fmt::println("Year_Doy Conversion Diff: {} ns", diff);

    ASSERT_LE(diff, 1);
}

TEST(DateTime, Unix_Timestamp_Conversion)
{
    DateTime dt{2025, 2, 7, 11, 30, 45};

    auto seconds = dt.unix_timestamp();
    DateTime dt2 = DateTime::from_unix_timestamp(seconds);

    auto diff = (dt.tp - dt2.tp).count();
    fmt::println("Unix Conversion Diff: {} ns", diff);

    ASSERT_LE(diff, 1);
}


TEST(DateTime, Julian_Date_Conversion)
{
    DateTime dt{2025, 2, 7, 11, 30, 45};

    auto days = dt.julian_date();
    DateTime dt2 = DateTime::from_julian_date(days);
    auto days2 = dt2.julian_date();

    auto diff = (dt.tp - dt2.tp).count();
    fmt::println("Julian Conversion Diff: {} ns", diff);

    ASSERT_LE(diff, 20000);
}