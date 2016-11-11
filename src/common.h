#ifndef COMMON_H_
#define COMMON_H_

#include "build_config.h"
#include <stdint.h>

#ifdef _WINDOWS
#   define eustia_api_export __declspec(dllexport)
#   define eustia_api_import __declspec(dllimport)

#   ifdef _USRDLL
#       define EUSTIA_DLLAPI eustia_api_export
#   else
#       define EUSTIA_DLLAPI eustia_api_import
#   endif // _USRDLL
#else
#   define EUSTIA_DLLAPI
#endif // _WINDOWS

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

namespace eustia
{

enum class ErrType
{
    Success,
    Fail,

    //basic errors
    NoMem,
    InvalidArg,

    //OS errors
    OSMem,
};

inline bool IsSuccess(ErrType tp)
{
    return tp == ErrType::Success;
}

enum class InjectType
{
    HookAuto, //e.g. callback
    HookManual, //e.g. keyboard, keyboardLL
    LoadLib, //CreateAndInject and OpenAndInject
};

enum class IPAddrType
{
    IPv4,
    IPv6,
};

}

#endif