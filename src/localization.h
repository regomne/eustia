#ifndef STRING_SUPPORT_H_
#define STRING_SUPPORT_H_

#include <string>

#include "src/common.h"

namespace eustia
{

std::u16string utf8_to_utf16(const std::string& str);

inline std::u16string utf8_to_utf16(const char* str, size_t len)
{
    return utf8_to_utf16(std::string(str, len));
}

template<size_t _Size>
inline std::u16string utf8_to_utf16(const char(&str)[_Size])
{
    return utf8_to_utf16(std::string(str, _Size));
}

inline std::u16string utf8_to_utf16(const char* str)
{
    return utf8_to_utf16(std::string(str));
}

std::string utf16_to_utf8(const std::u16string& str);

inline std::string utf16_to_utf8(const char16_t* str, size_t len)
{
    return utf16_to_utf8(std::u16string(str, len));
}

template<size_t _Size>
inline std::string utf16_to_utf8(const char16_t(&str)[_Size])
{
    return utf16_to_utf8(std::u16string(str, _Size));
}

inline std::string utf16_to_utf8(const char16_t* str)
{
    return utf16_to_utf8(std::u16string(str));
}

#ifdef _MSC_VER
inline std::string wchars_to_utf8(const wchar_t* str)
{
    return utf16_to_utf8(std::u16string(reinterpret_cast<const char16_t*>(str)));
}

// __forceinline const wchar_t* char16p_to_wcharp(const char16_t* p)
// {
//     return reinterpret_cast<const wchar_t*>(p);
// }
#endif

}

#endif