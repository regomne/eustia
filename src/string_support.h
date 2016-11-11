#ifndef STRING_SUPPORT_H_
#define STRING_SUPPORT_H_

#include <string>

#include "src/common.h"

namespace eustia
{

typedef std::string u8string;
typedef std::basic_string<char16_t, std::char_traits<char16_t>, std::allocator<char16_t>> u16string;
typedef std::basic_string<char32_t, std::char_traits<char32_t>, std::allocator<char32_t>> u32string;

inline size_t strlen16(const char16_t* string)
{
    size_t len = 0;
    while (string[len++]);
    return len;
}

inline size_t strlen32(const char32_t* string)
{
    size_t len = 0;
    while (string[len++]);
    return len;
}

u16string utf8_to_utf16(const char* str, size_t len);

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

}

#endif