#include "src/log.h"

#include <stdio.h>
#include <wtypes.h>
#include <time.h>

#include <windows.h>

#include "src/build_config.h"

namespace Eustia
{

EustiaLogger* EustiaLogger::instance_ = nullptr;

EustiaLogger* EustiaLogger::GetLogger()
{
    if (!instance_)
    {
        instance_ = new EustiaLogger();
    }
    return instance_;
}

EustiaLogger::EustiaLogger()
{
    currentLevel_ = LogLevelNothing;
}

EustiaLogger::~EustiaLogger()
{

}

void EustiaLogger::WriteLog(const char* fileName, int lineNum, LogLevel level, const char* format, ...)
{
    if (!IsNeedLog(level))
    {
        return;
    }

    auto fp = fopen(logFileName_.c_str(), "ab");
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
        GetLogLevelString(level),
        fileName,
        lineNum);

    va_list valist;
    va_start(valist, format);
    auto len2 = vsnprintf(buf + len, sizeof(buf) - len, format, valist);

    fwrite(buf, 1, len + len2, fp);
    fwrite("\r\n", 1, 2, fp);

    fclose(fp);
}

void EustiaLogger::WriteLog(LogLevel level, const char* format, ...)
{
    if (!IsNeedLog(level))
    {
        return;
    }

    auto fp = fopen(logFileName_.c_str(), "ab");
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
        GetLogLevelString(level));

    va_list valist;
    va_start(valist, format);
    auto len2 = vsnprintf(buf + len, sizeof(buf) - len, format, valist);

    fwrite(buf, 1, len + len2, fp);
    fwrite("\r\n", 1, 2, fp);

    fclose(fp);
}

const char* EustiaLogger::AnsiStr(const wchar* str)
{
    static char astr[1000];
    astr[0] = '\0';
    WideCharToMultiByte(CP_UTF8, 0, str, -1, astr, sizeof(astr), 0, 0);
    return astr;
}

void EustiaLogger::SetLogFileName(const wchar* logFileName)
{
    auto nameLen = WideCharToMultiByte(CP_ACP, 0, logFileName, -1, 0, 0, 0, 0);
    if (nameLen == 0)
    {
        return;
    }
    auto name = new char[nameLen];
    WideCharToMultiByte(CP_ACP, 0, logFileName, -1, name, nameLen, 0, 0);
    logFileName_ = name;
}

}