#include <windows.h>
#include <string>

#include "src/log.h"
#include "src/ipc.h"
#include "src/utils.h"
#include "src/HookDll/globalvars_h.h"

using namespace std;
using namespace Eustia;

static ErrType CheckNeedLoadEustia(const IPCInfo* info, 
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
        if ((intptr)GetCurrentProcessId() == info->destProcessId)
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
    auto dllName = wstring(GlobalVars::ModulePath) + L'/' + info->eustiaDllName;
    LoadLibrary(dllName.c_str());
}

static DWORD WINAPI MainProc(LPVOID _)
{
    UNREFERENCED_PARAMETER(_);
    static bool isInited = false;
    if (isInited)
    {
        return 0;
    }
    isInited = true;

    auto ipc = MemoryIPC::Init(MemoryIPC::InitType::OpenExisting, EUSTIA_COMM_MEMORY_NAME);
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
    GlobalVars::IpcInfo = *(IPCInfo*)ptr0;
    ipc->Dispose();

    bool isLoadImmediately;
    bool isNeedSelfUnload;
    auto ret = CheckNeedLoadEustia(&GlobalVars::IpcInfo, &isLoadImmediately, &isNeedSelfUnload, &GlobalVars::NeedDelayCheck);
    if (!IsSuccess(ret))
    {
        return 0;
    }
    if (isLoadImmediately)
    {
        LoadEustia(&GlobalVars::IpcInfo);
    }
    if (isNeedSelfUnload)
    {
        FreeLibraryAndExitThread((HMODULE)GlobalVars::DllHandle, 0);
    }

    return 0;
}

int WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    UNREFERENCED_PARAMETER(_Reserved);
    switch (_Reason)
    {
    case DLL_PROCESS_ATTACH:
        EustiaLogger::Init();
        GlobalVars::DllHandle = _DllHandle;
        GlobalVars::ModulePath = GetModulePathWin((HMODULE)_DllHandle);
        CloseHandle(CreateThread(0, 0, MainProc, 0, 0, 0));
        break;
    case DLL_PROCESS_DETACH:
        delete[] GlobalVars::ModulePath;
        GlobalVars::ModulePath = nullptr;
        EustiaLogger::Get()->Dispose();
        break;
    default:
        break;
    }
    return 0;
}