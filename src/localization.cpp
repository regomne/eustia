#include "src/localization.h"

#include <memory>

namespace eustia
{

struct _CheckType
{
#define CHAR_SIZE_ERROR "char size error"
    static_assert(sizeof(char32_t) == sizeof(llvm::UTF32), CHAR_SIZE_ERROR);
    static_assert(sizeof(char16_t) == sizeof(llvm::UTF16), CHAR_SIZE_ERROR);
    static_assert(sizeof(char) == sizeof(llvm::UTF8), CHAR_SIZE_ERROR);
#ifdef _WINDOWS
    static_assert(sizeof(wchar_t) == sizeof(char16_t), CHAR_SIZE_ERROR);
#endif
};

u16string utf8_to_utf16(const char* str, size_t len)
{
    auto src_ptr = (const llvm::UTF8*)str;
    auto buffer_len = len;
    auto dest = std::unique_ptr<llvm::UTF16[]>(new llvm::UTF16[buffer_len]);
    auto dest_ptr = dest.get();
    auto ret = llvm::ConvertUTF8toUTF16(
        &src_ptr, 
        src_ptr + len,
        &dest_ptr,
        dest_ptr + buffer_len,
        llvm::lenientConversion);
    if (ret == llvm::conversionOK)
    {
        return u16string((char16_t*)dest.get(), dest_ptr - dest.get());
    }
    return u16string(u"cvt error");
}

u8string utf16_to_utf8(const char16_t* str, size_t len)
{
    auto src_ptr = (const llvm::UTF16*)str;
    auto buffer_len = len * 6;
    auto dest = std::unique_ptr<llvm::UTF8[]>(new llvm::UTF8[buffer_len]);
    auto dest_ptr = dest.get();
    auto ret = llvm::ConvertUTF16toUTF8(
        &src_ptr,
        src_ptr + len,
        &dest_ptr,
        dest_ptr + buffer_len,
        llvm::lenientConversion);
    if (ret == llvm::conversionOK)
    {
        return u8string((char*)dest.get(), dest_ptr - dest.get());
    }
    return u8string("cvt error");
}

}