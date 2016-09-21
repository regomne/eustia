#ifndef COMMON_H_
#define COMMON_H_

#include "build_config.h"

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

namespace Eustia
{

typedef char i8;
typedef unsigned char u8;
typedef short i16;
typedef unsigned short u16;
typedef int i32;
typedef unsigned int u32;
typedef long long i64;
typedef unsigned long long u64;

#ifdef __X86
typedef int intptr;
#else
#ifdef __X64
typedef long long intptr;
#endif
#endif

#ifdef _MSC_VER
typedef wchar_t wchar;
#endif

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