#include "src/localization.h"

#include <memory>
#include <codecvt>

namespace eustia
{

std::u16string utf8_to_utf16(const std::string& str)
{
    //todo: the bug of vs2015. will fix under vs2017
    std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> ucconv;
    auto ustr = ucconv.from_bytes(str);
    return *reinterpret_cast<std::u16string*>(&ustr);
}

std::string utf16_to_utf8(const std::u16string& str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> ucconv;
    auto p = reinterpret_cast<const int16_t *>(str.data());
    return ucconv.to_bytes(p, p + str.size());
}

}