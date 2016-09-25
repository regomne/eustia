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

    enum LogPosition
    {
        LogPosFile,
        LogPosStderr,
        LogPosDebugPort,
    };

public:
    static void Init();
    static EustiaLogger* Get()
    {
        return instance_;
    }
    void Dispose();
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
    void SetLogLevel(LogLevel level)
    {
        logLevel_ = level;
    }
    void SetLogPosition(LogPosition pos)
    {
        logPosition_ = pos;
    }

    void WriteLog(const char* fileName, int lineNum, LogLevel level, const char* format, ...);
    void WriteLog(LogLevel level, const char* format, ...);

private:
    EustiaLogger();
    ~EustiaLogger();

    void WriteLogToFile(const char* fileName, int lineNum, LogLevel level, const char* format, va_list lst);
    void WriteLogToFile(LogLevel level, const char* format, va_list lst);
    void WriteLogToStderr(LogLevel level, const char* format, va_list lst);
    void WriteLogToDebugPort(LogLevel level, const char* format, va_list lst);

    inline const char* GetLogLevelString(LogLevel level) const
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

    inline bool IsNeedLog(LogLevel level) const
    {
        return logLevel_ >= level;
    }

private:
    static EustiaLogger* instance_;
    std::string logFileName_;
    LogLevel logLevel_;
    LogPosition logPosition_;
};

#ifdef BUILDCONFIG_LOG_WITH_FILENAME
#define FILENAME_PARAMETERS_IMPL __FILE__,__LINE__,
#else
#define FILENAME_PARAMETERS_IMPL
#endif

#define LOGERROR(format,...) Eustia::EustiaLogger::Get()->\
    WriteLog(FILENAME_PARAMETERS_IMPL Eustia::EustiaLogger::LogLevelError, format, __VA_ARGS__)
#define LOGINFO(format,...) Eustia::EustiaLogger::Get()->\
    WriteLog(FILENAME_PARAMETERS_IMPL Eustia::EustiaLogger::LogLevelInfo, format, __VA_ARGS__)
#define LOGDEBUG(format,...) Eustia::EustiaLogger::Get()->\
    WriteLog(FILENAME_PARAMETERS_IMPL Eustia::EustiaLogger::LogLevelDebug, format, __VA_ARGS__)

#define LOGASTR(str) Eustia::EustiaLogger::AnsiStr(str)

}

#endif