#pragma once

#include "datetime.hpp"
#include <chrono>



inline TimeUtils::Time operator+(const TimeUtils::Time& lhs, const TimeUtils::Time& rhs) {
    return TimeUtils::Time{lhs.sec + rhs.sec};
}

inline TimeUtils::Time operator-(const TimeUtils::Time& lhs, const TimeUtils::Time& rhs) {
    return TimeUtils::Time{lhs.sec - rhs.sec};
}

inline TimeUtils::Time& operator+=(TimeUtils::Time& lhs, const TimeUtils::Time& rhs) {
    lhs.sec += rhs.sec;
    return lhs;
}

inline TimeUtils::Time& operator-=(TimeUtils::Time& lhs, const TimeUtils::Time& rhs) {
    lhs.sec -= rhs.sec;
    return lhs;
}

// // TimeUtils::Date + TimeUtils::Date (interpreted as adding days from one to another)
// inline TimeUtils::Date operator+(const TimeUtils::Date& lhs, const TimeUtils::Date& rhs) {
//     auto lhs_days = std::chrono::sys_days{lhs};
//     auto rhs_days = std::chrono::sys_days{rhs};
//     return TimeUtils::Date{lhs_days + (rhs_days - std::chrono::sys_days{TimeUtils::Date(0,1,1)})}; // anchor at epoch
// }

// inline TimeUtils::Date operator-(const TimeUtils::Date& lhs, const TimeUtils::Date& rhs) {
//     auto lhs_days = std::chrono::sys_days{lhs};
//     auto rhs_days = std::chrono::sys_days{rhs};
//     return TimeUtils::Date{lhs_days - (rhs_days - std::chrono::sys_days{TimeUtils::Date(0,1,1)})};
// }

// inline TimeUtils::Date& operator+=(TimeUtils::Date& lhs, const TimeUtils::Date& rhs) {
//     lhs = lhs + rhs;
//     return lhs;
// }

// inline TimeUtils::Date& operator-=(TimeUtils::Date& lhs, const TimeUtils::Date& rhs) {
//     lhs = lhs - rhs;
//     return lhs;
// }


inline TimeUtils::DateTime operator+(const TimeUtils::Date& d, const TimeUtils::Time& t) { return TimeUtils::DateTime{d, t}; }
// inline TimeUtils::DateTime operator-(const TimeUtils::Date& d, const TimeUtils::Time& t) { return TimeUtils::DateTime{std::chrono::sys_days{d.ymd} - t.sec}; }
// inline TimeUtils::DateTime operator+(const TimeUtils::Time& t, const TimeUtils::Date& d) { return TimeUtils::DateTime{d, t}; }

inline TimeUtils::DateTime operator+(const TimeUtils::DateTime& dt, const TimeUtils::Time& t) { return TimeUtils::DateTime{dt.tp + t.sec}; }
inline TimeUtils::DateTime operator-(const TimeUtils::DateTime& dt, const TimeUtils::Time& t) { return TimeUtils::DateTime{dt.tp - t.sec}; }

inline TimeUtils::DateTime& operator+=(TimeUtils::DateTime& dt, const TimeUtils::Time& t) { dt.tp += t.sec; return dt; }
inline TimeUtils::DateTime& operator-=(TimeUtils::DateTime& dt, const TimeUtils::Time& t) { dt.tp -= t.sec; return dt; }

// inline TimeUtils::DateTime operator+(const TimeUtils::DateTime& dt, const TimeUtils::Date& d) {
//     return TimeUtils::DateTime{dt.tp + (std::chrono::sys_days{d} - std::chrono::sys_days{TimeUtils::Date(0,1,1)})};
// }

// inline TimeUtils::DateTime operator-(const TimeUtils::DateTime& dt, const TimeUtils::Date& d) {
//     return TimeUtils::DateTime{dt.tp - (std::chrono::sys_days{d} - std::chrono::sys_days{TimeUtils::Date(0,1,1)})};
// }

// inline TimeUtils::DateTime& operator+=(TimeUtils::DateTime& dt, const TimeUtils::Date& d) { dt = dt + d; return dt; }
// inline TimeUtils::DateTime& operator-=(TimeUtils::DateTime& dt, const TimeUtils::Date& d) { dt = dt - d; return dt; }

// inline TimeUtils::DateTime operator+(const TimeUtils::DateTime& a, const TimeUtils::DateTime& b) {
//     return TimeUtils::DateTime{a.tp + (b.tp - std::chrono::system_clock::time_point{})};
// }

// inline TimeUtils::DateTime operator-(const TimeUtils::DateTime& a, const TimeUtils::DateTime& b) {
//     return TimeUtils::DateTime{a.tp - (b.tp - std::chrono::system_clock::time_point{})};
// }

// inline TimeUtils::DateTime& operator+=(TimeUtils::DateTime& a, const TimeUtils::DateTime& b) { a = a + b; return a; }
// inline TimeUtils::DateTime& operator-=(TimeUtils::DateTime& a, const TimeUtils::DateTime& b) { a = a - b; return a; }