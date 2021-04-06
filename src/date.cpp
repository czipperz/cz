#include <cz/date.hpp>

#include <cz/defer.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#else
#define ZoneScoped
#endif

namespace cz {

bool get_file_time(const char* path, File_Time* file_time) {
    ZoneScoped;

#ifdef _WIN32
    HANDLE handle = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                               FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (handle != INVALID_HANDLE_VALUE) {
        CZ_DEFER(CloseHandle(handle));
        if (GetFileTime(handle, NULL, NULL, &file_time->data)) {
            return true;
        }
    }
    return false;
#else
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    file_time->data = st.st_mtime;
    return true;
#endif
}

bool is_file_time_before(File_Time file_time, File_Time other_file_time) {
#ifdef _WIN32
    return CompareFileTime(&file_time.data, &other_file_time.data) < 0;
#else
    return file_time.data < other_file_time.data;
#endif
}

Date tm_to_date(struct tm tm) {
    Date date;
    date.year = tm.tm_year + 1900;
    date.month = tm.tm_mon + 1;
    date.day_of_month = tm.tm_mday;
    date.day_of_week = tm.tm_wday;
    date.hour = tm.tm_hour;
    date.minute = tm.tm_min;
    date.second = tm.tm_sec;
    return date;
}

struct tm time_t_to_tm_utc(time_t time) {
    // Try to use thread safe versions of gmtime when possible.
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE || _POSIX_SOURCE
    struct tm storage;
    struct tm* tm = gmtime_r(&time, &storage);
#elif defined(__STDC_LIB_EXT1__)
    struct tm storage;
    struct tm* tm = gmtime_s(&time, &storage);
#else
    struct tm* tm = gmtime(&time);
#endif
    return *tm;
}

struct tm time_t_to_tm_local(time_t time) {
    // Try to use thread safe versions of localtime when possible.
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE || _POSIX_SOURCE
    struct tm storage;
    struct tm* tm = localtime_r(&time, &storage);
#elif defined(__STDC_LIB_EXT1__)
    struct tm storage;
    struct tm* tm = localtime_s(&time, &storage);
#else
    struct tm* tm = localtime(&time);
#endif
    return *tm;
}

bool file_time_to_date_utc(File_Time file_time, Date* date) {
#ifdef _WIN32
    SYSTEMTIME system_time;
    if (!FileTimeToSystemTime(&file_time.data, &system_time)) {
        return false;
    }
    date->year = system_time.wYear;
    date->month = system_time.wMonth;
    date->day_of_month = system_time.wDay;
    date->day_of_week = system_time.wDayOfWeek;
    date->hour = system_time.wHour;
    date->minute = system_time.wMinute;
    date->second = system_time.wSecond;
    return true;
#else
    *date = time_t_to_date_utc(file_time.data);
    return true;
#endif
}

bool file_time_to_date_local(File_Time file_time, Date* date) {
#ifdef _WIN32
    File_Time local_time;
    if (!FileTimeToLocalFileTime(&file_time.data, &local_time.data)) {
        return false;
    }
    return file_time_to_date_utc(local_time, date);
#else
    *date = time_t_to_date_local(file_time.data);
    return true;
#endif
}

Date time_t_to_date_utc(time_t t) {
    return tm_to_date(time_t_to_tm_utc(t));
}

Date time_t_to_date_local(time_t t) {
    return tm_to_date(time_t_to_tm_local(t));
}

}
