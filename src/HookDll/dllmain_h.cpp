#include <windows.h>

#include "src/ipc.h"

using namespace Eustia;

HANDLE g_DllHandle;
bool g_NeedDelayCheck;
intptr g_CheckProcessId;

static ErrType CheckNeedLoadEustia(IPCInfo* info, 
    bool* isLoadImmediately, 
    bool* isNeedSelfUnload,
    bool* isNeedDelayCheck) //check when hook callback runs
{
    *isLoadImmediately = false;
    *isNeedSelfUnload = false;
    *isNeedDelayCheck = false;
    switch (info->injectType)
    {
    case InjectType::HookAuto:
        if (GetCurrentProcessId() == info->destProcessId)
        {
            *isLoadImmediately = true;
        }
        break;
    case InjectType::HookManual:
        *isNeedDelayCheck = true;
        break;
    case InjectType::LoadLib:
        *isNeedSelfUnload = true;
        *isLoadImmediately = true;
        break;
    default:
        return ErrType::InvalidArg;
    }
    return ErrType::Success;
}

void LoadEustia(IPCInfo* info)
{

}

static DWORD WINAPI MainProc(LPVOID _)
{
    static bool isInited = false;
    if (isInited)
    {
        return 0;
    }
    isInited = true;

    auto ipc = MemoryIPC::InitOneIPC(MemoryIPC::InitType::OpenExisting, EUSTIA_COMM_MEMORY_NAME);
    if (!ipc)
    {
        //OutputDebugStringW(L"InitOneIPC Fail");
        return 0;
    }

    void* ptr0;
    if (!IsSuccess(ipc->GetMemoryPointer(&ptr0)))
    {
        //OutputDebugStringW(L"GetMemoryPointer Fail");
        ipc->Dispose();
        return 0;
    }

    auto ipcInfo = (IPCInfo*)ptr0;
    bool isLoadImmediately;
    bool isNeedSelfUnload;
    auto ret = CheckNeedLoadEustia(ipcInfo, &isLoadImmediately, &isNeedSelfUnload, &g_NeedDelayCheck);
    if (!IsSuccess(ret))
    {
        return 0;
    }

    if (isLoadImmediately)
    {
        LoadEustia(ipcInfo);
    }

    if (g_NeedDelayCheck)
    {
        g_CheckProcessId = ipcInfo->destProcessId;
    }

    if (isNeedSelfUnload)
    {

    }

    ipc->Dispose();
    return 0;
}

int WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    UNREFERENCED_PARAMETER(_Reserved);
    switch (_Reason)
    {
    case DLL_PROCESS_ATTACH:
        g_DllHandle = _DllHandle;
        CreateThread(0, 0, MainProc, 0, 0, 0);
        break;
    default:
        break;
    }
    return 0;
}