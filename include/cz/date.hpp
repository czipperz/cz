#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include <time.h>

namespace cz {

struct File_Time {
#ifdef _WIN32
    FILETIME data;
#else
    time_t data;
#endif
};

bool get_file_time(const char* path, File_Time* file_time);
bool is_file_time_before(File_Time file_time, File_Time other_file_time);

struct Date {
    /// Printable year (2020 = year 2020).
    int year;
    /// Printable month (1 = January).
    int month;
    /// Printable day of month (1 = 1st).
    int day_of_month;
    /// Index day of week (0 = Sunday).
    int day_of_week;
    /// Index hour (0 = 01:00:00 AM).
    int hour;
    /// Index minute (0 = 00:01:00).
    int minute;
    /// Index second (0 = 00:00:01).
    int second;
};

/// Convert a `file_time` to a `Date` in UTC.
bool file_time_to_date_utc(File_Time file_time, Date* date);
/// Convert a `file_time` to a `Date` in the local time zone.
bool file_time_to_date_local(File_Time file_time, Date* date);

struct tm time_t_to_tm_utc(time_t);
struct tm time_t_to_tm_local(time_t);
Date tm_to_date(struct tm);

}
