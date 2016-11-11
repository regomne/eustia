#ifndef GLOBALVARS_H_H_
#define GLOBALVARS_H_H_

#include "src/common.h"
#include "src/ipc.h"

#include <windows.h>

class GlobalVars
{
public:
    static eustia::wchar* ModulePath;
    static HANDLE DllHandle;
    static bool NeedDelayCheck;
    static eustia::intptr CheckProcessId;
    static eustia::IPCInfo IpcInfo;
};


#endif