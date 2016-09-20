#ifndef LOG_H_
#define LOG_H_

#include <string>

#include "src/build_config.h"
#include "src/common.h"

namespace Eustia
{

class EustiaLogger
{
public:
    enum LogLevel
    {
        LogLevelNothing,
        LogLevelError,
        LogLevelInfo,
        LogLevelDebug,
    };

public:
    static EustiaLogger* GetLogger();
    static const char* AnsiStr(const wchar* str);

    void SetLogFileName(const char* logFileName)
    {
        //this function can only be set once.
        if (logFileName_ == "")
        {
            logFileName_ = logFileName;
        }
    }
    void SetLogFileName(const wchar* logFileName);
    void SetCurrentLogLevel(LogLevel level)
    {
        currentLevel_ = level;
    }

    void WriteLog(const char* fileName, int lineNum, LogLevel level, const char* format, ...);
    void WriteLog(LogLevel level, const char* format, ...);

private:
    EustiaLogger();
    ~EustiaLogger();

    inline const char* GetLogLevelString(LogLevel level)
    {
        switch (level)
        {
        case Eustia::EustiaLogger::LogLevelDebug:
            return "DEBUG";
        case Eustia::EustiaLogger::LogLevelInfo:
            return "INFO ";
        case Eustia::EustiaLogger::LogLevelError:
            return "ERROR";
        }
        return "UNK  ";
    }

    inline bool IsNeedLog(LogLevel level)
    {
        return currentLevel_ >= level;
    }

private:
    static EustiaLogger* instance_;
    std::string logFileName_;
    LogLevel currentLevel_;
};

#ifdef BUILDCONFIG_LOG_WITH_FILENAME
#define FILENAME_PARAMETERS_IMPL __FILE__,__LINE__,
#else
#define FILENAME_PARAMETERS_IMPL
#endif

#define LOGERROR(format,...) Eustia::EustiaLogger::GetLogger()->\
    WriteLog(FILENAME_PARAMETERS_IMPL Eustia::EustiaLogger::LogLevelError, format, __VA_ARGS__)
#define LOGINFO(format,...) Eustia::EustiaLogger::GetLogger()->\
    WriteLog(FILENAME_PARAMETERS_IMPL Eustia::EustiaLogger::LogLevelInfo, format, __VA_ARGS__)
#define LOGDEBUG(format,...) Eustia::EustiaLogger::GetLogger()->\
    WriteLog(FILENAME_PARAMETERS_IMPL Eustia::EustiaLogger::LogLevelDebug, format, __VA_ARGS__)

#define LOGASTR(str) Eustia::EustiaLogger::AnsiStr(str)

}

#endif