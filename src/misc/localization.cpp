#include "src/misc/localization.h"

#include <memory>
#include <codecvt>

namespace eustia
{

std::u16string utf8_to_utf16(const char* str, size_t len)
{
    //todo: the bug of vs2015. will fix under vs2017
    std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> ucconv;
    auto ustr = ucconv.from_bytes(str, str + len);
    return *(std::u16string*)&ustr;
}

std::string utf16_to_utf8(const char16_t* str, size_t len)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> ucconv;
    return ucconv.to_bytes((char16_t*)str, (char16_t*)(str + len));
}

}