#ifndef UNICODE_SUPPORT_H_
#define UNICODE_SUPPORT_H_

#include <string>
#include <string.h>

#include "src/common.h"

namespace Eustia
{

std::wstring Utf8ToUtf16(const char* str, u32 len);

inline std::wstring Utf8ToUtf16(const char* str)
{
    return Utf8ToUtf16(str, strlen(str));
}

inline std::wstring Utf8ToUtf16(const std::string& str)
{
    return Utf8ToUtf16(str.c_str(), str.length());
}

std::string Utf16ToUtf8(const wchar* str, u32 lenOfChars);

inline std::string Utf16ToUtf8(const wchar* str)
{
    return Utf16ToUtf8(str, wcslen(str));
}

inline std::string Utf16ToUtf8(const std::wstring& str)
{
    return Utf16ToUtf8(str.c_str(), str.length());
}

}

#endif