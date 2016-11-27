#ifndef STRING_SUPPORT_H_
#define STRING_SUPPORT_H_

#include <string>

#include "src/common.h"

namespace eustia
{

class LocaleHelper
{
public:
    static LocaleHelper* get()
    {
        return instance;
    }

private:
    static LocaleHelper* instance;
};

std::u16string utf8_to_utf16(const char* str, size_t len);

template<size_t _Size>
inline u16string utf8_to_utf16(const char(&str)[_Size])
{
    return utf8_to_utf16(str, _Size);
}

inline u16string utf8_to_utf16(const char* str)
{
    return utf8_to_utf16(str, strlen(str));
}

inline u16string utf8_to_utf16(const u8string& str)
{
    return utf8_to_utf16(str.c_str(), str.length());
}

u8string utf16_to_utf8(const char16_t* str, size_t len);

template<size_t _Size>
inline u8string utf16_to_utf8(const char16_t(&str)[_Size])
{
    return utf16_to_utf8(str, _Size);
}

inline u8string utf16_to_utf8(const char16_t* str)
{
    return utf16_to_utf8(str, strlen16(str));
}

inline u8string utf16_to_utf8(const u16string& str)
{
    return utf16_to_utf8(str.c_str(), str.length());
}

#ifdef _MSC_VER
__forceinline u8string wchars_to_utf8(const wchar_t* str)
{
    return utf16_to_utf8(reinterpret_cast<const char16_t*>(str));
}

__forceinline const wchar_t* char16p_to_wcharp(const char16_t* p)
{
    return reinterpret_cast<const wchar_t*>(p);
}
#endif

}

#endif