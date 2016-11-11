#ifndef LOG_H_
#define LOG_H_

#include <string>

#include "src/build_config.h"
#include "src/common.h"

namespace eustia
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
    static void init();
    static EustiaLogger* get()
    {
        return instance_;
    }
    void dispose();
    static const char* ansi_str(const wchar* str);

    void set_log_file_name(const char* logFileName)
    {
        //this function can only be set once.
        if (log_file_name_ == "")
        {
            log_file_name_ = logFileName;
        }
    }
    void set_log_file_name(const wchar* logFileName);
    void set_log_level(LogLevel level)
    {
        log_level_ = level;
    }
    void set_log_position(LogPosition pos)
    {
        log_position_ = pos;
    }

    void write_log(const char* fileName, int lineNum, LogLevel level, const char* format, ...);
    void write_log(LogLevel level, const char* format, ...);
    void write_to_stdout(const char* format, ...);

private:
    EustiaLogger();
    ~EustiaLogger();

    void write_log_to_file(const char* fileName, int lineNum, LogLevel level, const char* format, va_list lst);
    void write_log_to_file(LogLevel level, const char* format, va_list lst);
    void write_log_to_stderr(LogLevel level, const char* format, va_list lst);
    void write_log_to_debug_port(LogLevel level, const char* format, va_list lst);

    inline const char* get_log_level_string(LogLevel level) const
    {
        switch (level)
        {
        case eustia::EustiaLogger::LogLevelDebug:
            return "DEBUG";
        case eustia::EustiaLogger::LogLevelInfo:
            return "INFO ";
        case eustia::EustiaLogger::LogLevelError:
            return "ERROR";
        }
        return "UNK  ";
    }

    inline bool is_need_log(LogLevel level) const
    {
        return log_level_ >= level;
    }

private:
    static EustiaLogger* instance_;
    std::string log_file_name_;
    LogLevel log_level_;
    LogPosition log_position_;
};

#ifdef BUILDCONFIG_LOG_WITH_FILENAME
#define FILENAME_PARAMETERS_IMPL __FILE__,__LINE__,
#else
#define FILENAME_PARAMETERS_IMPL
#endif

#define LOGERROR(format,...) eustia::EustiaLogger::get()->\
    write_log(FILENAME_PARAMETERS_IMPL eustia::EustiaLogger::LogLevelError, format, __VA_ARGS__)
#define LOGINFO(format,...) eustia::EustiaLogger::get()->\
    write_log(FILENAME_PARAMETERS_IMPL eustia::EustiaLogger::LogLevelInfo, format, __VA_ARGS__)
#define LOGDEBUG(format,...) eustia::EustiaLogger::get()->\
    write_log(FILENAME_PARAMETERS_IMPL eustia::EustiaLogger::LogLevelDebug, format, __VA_ARGS__)
#define OUT_STD(format,...) eustia::EustiaLogger::get()->\
    write_to_stdout(format, __VA_ARGS__)

#define LOGASTR(str) eustia::EustiaLogger::ansi_str(str)

}

#endif