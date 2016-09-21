#include "src/utils.h"

#include <windows.h>

#include "src/log.h"

namespace Eustia
{

wchar* GetModulePath(HMODULE mod)
{
    size_t bufferLen = 257;
    wchar* exeName = new wchar[bufferLen];
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
            exeName = new wchar[bufferLen];
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
    return exeName;
}

intptr GetProcessIdentifer()
{
    return GetCurrentProcessId();
}

}