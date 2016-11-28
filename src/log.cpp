#include "src/log.h"

#include <cstdio>
#include <ctime>
#include <cstdarg>

#ifdef _WINDOWS
#include <windows.h>
#endif

#include "src/build_config.h"

using namespace std;

namespace eustia
{

EustiaLogger* EustiaLogger::instance_ = nullptr;

EustiaLogger::EustiaLogger()
{
    log_level_ = LogLevelNothing;
    log_position_ = LogPosFile;
}

EustiaLogger::~EustiaLogger()
{

}

void EustiaLogger::write_log(const char* fileName, int lineNum, LogLevel level, const char* format, ...)
{
    if (!is_need_log(level))
    {
        return;
    }
    va_list valist;
    va_start(valist, format);
    switch (log_position_)
    {
    case eustia::EustiaLogger::LogPosFile:
        write_log_to_file(fileName, lineNum, level, format, valist);
        break;
    case eustia::EustiaLogger::LogPosStderr:
        write_log_to_stderr(level, format, valist);
        break;
    case eustia::EustiaLogger::LogPosDebugPort:
        write_log_to_debug_port(level, format, valist);
        break;
    default:
        break;
    }
    return;
}

void EustiaLogger::write_log(LogLevel level, const char* format, ...)
{
    if (!is_need_log(level))
    {
        return;
    }
    va_list valist;
    va_start(valist, format);
    switch (log_position_)
    {
    case eustia::EustiaLogger::LogPosFile:
        write_log_to_file(level, format, valist);
        break;
    case eustia::EustiaLogger::LogPosStderr:
        write_log_to_stderr(level, format, valist);
        break;
    case eustia::EustiaLogger::LogPosDebugPort:
        write_log_to_debug_port(level, format, valist);
        break;
    default:
        break;
    }
    return;
}

void EustiaLogger::write_log_to_file(const char* fileName, int lineNum, LogLevel level, const char* format, va_list lst)
{
    auto fp = fopen(log_file_name_.c_str(), "ab");
    if (!fp)
    {
        return;
    }

    char buf[1000];
    time_t curTime;

    time(&curTime);
    auto localTime = localtime(&curTime);
    auto len = snprintf(buf,
        sizeof(buf),
        "%04d-%02d-%02d %02d:%02d:%02d | %s | %s:%d | ",
        localTime->tm_year + 1900,
        localTime->tm_mon + 1,
        localTime->tm_mday,
        localTime->tm_hour,
        localTime->tm_min,
        localTime->tm_sec,
        get_log_level_string(level),
        fileName,
        lineNum);

    auto len2 = vsnprintf(buf + len, sizeof(buf) - len, format, lst);

    fwrite(buf, 1, len + len2, fp);
    fwrite("\r\n", 1, 2, fp);

    fclose(fp);
}

void EustiaLogger::write_log_to_file(LogLevel level, const char* format, va_list lst)
{

    auto fp = fopen(log_file_name_.c_str(), "ab");
    if (!fp)
    {
        return;
    }

    char buf[1000];
    time_t curTime;

    time(&curTime);
    auto localTime = localtime(&curTime);
    auto len = snprintf(buf,
        sizeof(buf),
        "%04d-%02d-%02d %02d:%02d:%02d | %s | ",
        localTime->tm_year + 1900,
        localTime->tm_mon + 1,
        localTime->tm_mday,
        localTime->tm_hour,
        localTime->tm_min,
        localTime->tm_sec,
        get_log_level_string(level));

    auto len2 = vsnprintf(buf + len, sizeof(buf) - len, format, lst);

    fwrite(buf, 1, len + len2, fp);
    fwrite("\r\n", 1, 2, fp);

    fclose(fp);
}

void EustiaLogger::write_log_to_stderr(LogLevel level, const char* format, va_list lst)
{
    char buf[1000];
    auto len = snprintf(buf, sizeof(buf), "%s | ", get_log_level_string(level));
    vsnprintf(buf + len, sizeof(buf) - len, format, lst);
    fprintf(stderr, "%s\n", buf);
}

#ifdef _WINDOWS
void EustiaLogger::write_log_to_debug_port(LogLevel level, const char* format, va_list lst)
{
    char buf[1000];
    auto len = snprintf(buf, sizeof(buf), "%s | ", get_log_level_string(level));
    vsnprintf(buf + len, sizeof(buf) - len, format, lst);
    OutputDebugStringA(buf);
}
#endif

const char* EustiaLogger::ansi_str(const wchar_t* str)
{
    static char astr[1000];
    astr[0] = '\0';
    WideCharToMultiByte(CP_UTF8, 0, str, -1, astr, sizeof(astr), 0, 0);
    
    return astr;
}

void EustiaLogger::set_log_file_name(const wchar_t* logFileName)
{
    auto nameLen = WideCharToMultiByte(CP_ACP, 0, logFileName, -1, 0, 0, 0, 0);
    if (nameLen == 0)
    {
        return;
    }
    auto name = new char[nameLen];
    WideCharToMultiByte(CP_ACP, 0, logFileName, -1, name, nameLen, 0, 0);
    log_file_name_ = name;
}

void EustiaLogger::dispose()
{
    instance_ = nullptr;
    delete this;
}

}