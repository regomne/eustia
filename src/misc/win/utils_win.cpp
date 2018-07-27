#include "src/utils.h"

#include <windows.h>

#include "src/log.h"
#include "src/localization.h"

namespace eustia
{

std::string get_module_path_win(HMODULE mod)
{
    size_t bufferLen = 257;
    wchar_t* exeName = new wchar_t[bufferLen];
    do
    {
        auto len = GetModuleFileName(mod, exeName, bufferLen);
        if (len == 0)
        {
            //LOGERROR("GetModuleFileName fail. err:%d", GetLastError());
            delete[] exeName;
            return nullptr;
        }
        if (len == bufferLen && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            bufferLen *= 2;
            delete[] exeName;
            exeName = new wchar_t[bufferLen];
            continue;
        }
    } while (false);
    auto posPtr = wcsrchr(exeName, L'\\');
    if (!posPtr)
    {
        //LOGERROR("Can't find '\\' in module file name: %s", LOGASTR(exeName));
        delete[] exeName;
        return nullptr;
    }
    *(posPtr + 1) = L'\0';
    return wchars_to_utf8(exeName);
}

std::string get_this_module_path()
{
    return get_module_path_win(GetModuleHandle(NULL));
}

intptr_t get_process_identifier()
{
    return GetCurrentProcessId();
}

}