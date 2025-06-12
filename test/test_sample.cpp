#include <fmt/core.h>

#include <gtest/gtest.h>

#include "date.hpp"
#include "time.hpp"
#include "datetime.hpp"
#include "datetime_operators.hpp"

using namespace TimeUtils;

TEST(DateTime, construction)
{
    Date d = {2025, 6, 11};
    Time t = {14, 30, 45};

    DateTime dt = d + t;

    dt.print_all();
}