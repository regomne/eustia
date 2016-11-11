#ifndef GLOBALVARS_H_H_
#define GLOBALVARS_H_H_

#include "src/common.h"
#include "src/ipc.h"

#include <windows.h>

class GlobalVars
{
public:
    static wchar_t* ModulePath;
    static HANDLE DllHandle;
    static bool NeedDelayCheck;
    static intptr_t CheckProcessId;
    static eustia::IPCInfo IpcInfo;
};


#endif