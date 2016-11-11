#include <windows.h>
#include <string>

#include "src/log.h"
#include "src/ipc.h"
#include "src/utils.h"
#include "src/HookDll/globalvars_h.h"

using namespace std;
using namespace eustia;

static ErrType CheckNeedLoadEustia(const IPCInfo* info, 
    bool* isLoadImmediately, 
    bool* isNeedSelfUnload,
    bool* isNeedDelayCheck) //check when hook callback runs
{
    *isLoadImmediately = false;
    *isNeedSelfUnload = false;
    *isNeedDelayCheck = false;
    switch (info->inject_type)
    {
    case InjectType::HookAuto:
        if ((intptr)GetCurrentProcessId() == info->dest_process_id)
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
    auto dllName = wstring(GlobalVars::ModulePath) + L'/' + info->eusita_dll_name;
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

    auto ipc = MemoryIPC::init(MemoryIPC::InitType::OpenExisting, EUSTIA_COMM_MEMORY_NAME);
    if (!ipc)
    {
        //OutputDebugStringW(L"InitOneIPC Fail");
        return 0;
    }
    void* ptr0;
    if (!IsSuccess(ipc->get_memory_pointer(&ptr0)))
    {
        //OutputDebugStringW(L"GetMemoryPointer Fail");
        ipc->dispose();
        return 0;
    }
    GlobalVars::IpcInfo = *(IPCInfo*)ptr0;
    ipc->dispose();

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
        EustiaLogger::init();
        GlobalVars::DllHandle = _DllHandle;
        GlobalVars::ModulePath = get_module_path_win((HMODULE)_DllHandle);
        CloseHandle(CreateThread(0, 0, MainProc, 0, 0, 0));
        break;
    case DLL_PROCESS_DETACH:
        delete[] GlobalVars::ModulePath;
        GlobalVars::ModulePath = nullptr;
        EustiaLogger::get()->dispose();
        break;
    default:
        break;
    }
    return 0;
}