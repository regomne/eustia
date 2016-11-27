#include "src/Injector/injector.h"

#include <windows.h>
#include <assert.h>

#include "src/common.h"
#include "src/log.h"
#include "src/string_support.h"

using namespace eustia;


namespace injector
{

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWCH   Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

extern "C"
{
    NTSYSAPI NTSTATUS NTAPI NtSetContextThread(
        IN HANDLE ThreadHandle,
        IN CONTEXT* Context);

    NTSYSAPI NTSTATUS NTAPI NtGetContextThread(
        IN HANDLE ThreadHandle,
        OUT CONTEXT* pContext);

    NTSYSAPI NTSTATUS WINAPI LdrLoadDll(
        IN PWCHAR PathToFile OPTIONAL,
        IN ULONG Flags OPTIONAL,
        IN PUNICODE_STRING ModuleFileName,
        OUT PHANDLE ModuleHandle);
}

typedef NTSTATUS(WINAPI *LdrLoadDllRoutine)(PWCHAR,ULONG,PUNICODE_STRING,PHANDLE);

struct ProcInfo
{
    LdrLoadDllRoutine LdrLoadDllFunc;
    HANDLE dllHandle;
    UNICODE_STRING dllName;
};

#ifdef __X86

__declspec(naked) void LoadLib()
{
    __asm {
        pushad;
        call lbl;
    lbl:
        pop ecx;
        lea eax, [ecx - SIZE ProcInfo - 6]ProcInfo.dllHandle;
        push eax;
        lea eax, [ecx - SIZE ProcInfo - 6]ProcInfo.dllName;
        push eax;
        xor eax, eax;
        push eax;
        push eax;
        call[ecx - SIZE ProcInfo - 6]ProcInfo.LdrLoadDllFunc;
        popad;
        mov eax, 0xffffffff; //this instruction must be the last in this function.
    }
}

uint32_t GetLoadLibLength(intptr_t* jmpPos)
{
    //assume length of Loadlib() less than 0x50 bytes
    for (auto p = (uint8_t*)LoadLib;p < (uint8_t*)LoadLib + 0x50;p++)
    {
        //search mov eax,0xffffffff
        if (*p == 0xb8 && *(uint32_t*)(p + 1) == 0xffffffff)
        {
            *jmpPos = (intptr_t)(p - (uint8_t*)LoadLib);
            return p + 5 - (uint8_t*)LoadLib;
        }
    }
    //never go to here.
    assert(false);
    return 0;
}

//we write this to hp:
// ProcInfo
// LoadLib()
// pad bytes
// dllNameString

bool InjectStartingProcess(HANDLE hp, HANDLE ht, const wchar_t* dllPath)
{
    ProcInfo procInfo;
    intptr_t jmpPos;
    auto loadLibLength = GetLoadLibLength(&jmpPos);
    auto dllPathLength = wcslen(dllPath);
    auto loadLibStartOffset = sizeof(procInfo);
    auto dllPathStartOffset = loadLibStartOffset + loadLibLength;
    //align to 4 bytes
    dllPathStartOffset = (dllPathStartOffset & 3) ? ((dllPathStartOffset & ~3) + 4) : dllPathStartOffset;
    auto codeBuffer = (uint8_t*)VirtualAllocEx(
        hp,
        0,
        dllPathStartOffset + dllPathLength * sizeof(WCHAR),
        MEM_COMMIT,
        PAGE_EXECUTE_READWRITE);
    if (!codeBuffer)
    {
        LOGERROR("Can't alloc mem in dest process.");
        return false;
    }

    procInfo.LdrLoadDllFunc = LdrLoadDll;
    procInfo.dllName.Buffer = (PWCH)(codeBuffer + dllPathStartOffset);
    procInfo.dllName.Length = (USHORT)(dllPathLength * sizeof(WCHAR));
    procInfo.dllName.MaximumLength = (USHORT)(dllPathLength * sizeof(WCHAR));

    CONTEXT ctt;
    ctt.ContextFlags = CONTEXT_CONTROL;

    NTSTATUS st = NtGetContextThread(ht, &ctt);
    if (st != 0)
    {
        LOGERROR("Can't get context of thread. err:%d", GetLastError());
        VirtualFreeEx(hp, codeBuffer, 0, MEM_RELEASE);
        return false;
    }
    uint8_t jmpGates[5] = { 0xe9,0,0,0,0 };
    *(uint32_t*)&jmpGates[1] = ctt.Eip - (uint32_t)(codeBuffer + loadLibStartOffset + jmpPos + 5);

    SIZE_T bytesWrote = 0;
    bool wroteSuccess = true;
    wroteSuccess = wroteSuccess && WriteProcessMemory(hp, codeBuffer, &procInfo, sizeof(procInfo), &bytesWrote);
    wroteSuccess = wroteSuccess && WriteProcessMemory(hp, codeBuffer + loadLibStartOffset, &LoadLib, jmpPos, &bytesWrote);
    wroteSuccess = wroteSuccess && WriteProcessMemory(hp, codeBuffer + loadLibStartOffset + jmpPos, jmpGates, 5, &bytesWrote);
    wroteSuccess = wroteSuccess && WriteProcessMemory(hp, codeBuffer + dllPathStartOffset, dllPath, dllPathLength * sizeof(wchar_t), &bytesWrote);

    if (!wroteSuccess)
    {
        LOGERROR("Write process memory fail.");
        VirtualFreeEx(hp, codeBuffer, 0, MEM_RELEASE);
        return false;
    }

    ctt.Eip = (DWORD)(codeBuffer + loadLibStartOffset);
    st = NtSetContextThread(ht, &ctt);
    if (st != 0)
    {
        LOGERROR("Can't set context of thread.");
        VirtualFreeEx(hp, codeBuffer, 0, MEM_RELEASE);
        return false;
    }
    return true;
}

//inject dll when process is to start
bool create_and_inject(const std::string& app_path, const std::string& params, const std::string& mod_path)
{
    auto u16_app_path = utf8_to_utf16(app_path);
    auto u16_params = utf8_to_utf16(params);
    auto u16_mod_path = utf8_to_utf16(mod_path);

    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    if (GetFileAttributes(char16p_to_wcharp(u16_app_path.c_str())) == INVALID_FILE_ATTRIBUTES)
    {
        LOGERROR("Can't find %s", LOGASTR(char16p_to_wcharp(u16_app_path.c_str())));
        return false;
    }

    if (!CreateProcess(0, app_name, 0, 0, FALSE, CREATE_SUSPENDED, 0, 0, &si, &pi))
    {
        auto err = GetLastError();
        LOGERROR("Can't create process: %s, error:%d.", LOGASTR(app_name), err);
        return false;
    }

    if (!InjectStartingProcess(pi.hProcess, pi.hThread, mod_name))
    {
        TerminateProcess(pi.hProcess, 0);
        return false;
    }

    ResumeThread(pi.hThread);

    return true;
}
#endif // __X86

bool adjust_process_token_privilege()
{
    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        LOGINFO("OpenProcessToken fail. err:%d", GetLastError());
        return false;
    }

    LUID luidTmp;
    if (!LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &luidTmp))
    {
        LOGINFO("LookupPrivilegeValue fail. err:%d", GetLastError());
        CloseHandle(hToken);
        return false;
    }

    TOKEN_PRIVILEGES tkp;
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = luidTmp;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), nullptr, nullptr))
    {
        LOGINFO("AdjustTokenPrivileges fail. err:%d", GetLastError());
        CloseHandle(hToken);
        return false;
    }
    LOGDEBUG("self process privilege adjusted.");
    return true;
}

//assume length of this function less than 200bytes.
static const size_t LenOfRemoteThreadProc = 208;
static DWORD WINAPI RemoteThreadProc(LPVOID param)
{
    auto procInfo = (ProcInfo*)param;
    procInfo->LdrLoadDllFunc(0, 0, &procInfo->dllName, &procInfo->dllHandle);
    return 0;
}

bool InjectRunningProcess(HANDLE hp, const wchar_t* dllName)
{
    ProcInfo procInfo;
    auto dllPathLength = wcslen(dllName);
    auto remoteThreadStartOffset = sizeof(procInfo);
    auto dllPathStartOffset = remoteThreadStartOffset + LenOfRemoteThreadProc;
    dllPathStartOffset = (dllPathStartOffset & 3) ? ((dllPathStartOffset & ~3) + 4) : dllPathStartOffset;
    auto codeBuffer = (uint8_t*)VirtualAllocEx(
        hp,
        0,
        dllPathStartOffset + dllPathLength * sizeof(WCHAR),
        MEM_COMMIT,
        PAGE_EXECUTE_READWRITE);
    if (!codeBuffer)
    {
        LOGERROR("Can't alloc mem in dest process.");
        return false;
    }

    procInfo.LdrLoadDllFunc = LdrLoadDll;
    procInfo.dllName.Buffer = (PWCH)(codeBuffer + dllPathStartOffset);
    procInfo.dllName.Length = (USHORT)(dllPathLength * sizeof(WCHAR));
    procInfo.dllName.MaximumLength = (USHORT)(dllPathLength * sizeof(WCHAR));

    SIZE_T bytesWrote;
    bool wroteSuccess = true;
    wroteSuccess = wroteSuccess && WriteProcessMemory(hp, codeBuffer, &procInfo, sizeof(procInfo), &bytesWrote);
    wroteSuccess = wroteSuccess && WriteProcessMemory(hp, codeBuffer + remoteThreadStartOffset, &RemoteThreadProc, LenOfRemoteThreadProc, &bytesWrote);
    wroteSuccess = wroteSuccess && WriteProcessMemory(hp, codeBuffer + dllPathStartOffset, dllName, dllPathLength * sizeof(wchar_t), &bytesWrote);
    if (!wroteSuccess)
    {
        LOGERROR("Write process memory fail.");
        VirtualFreeEx(hp, codeBuffer, 0, MEM_RELEASE);
        return false;
    }

    auto thread = CreateRemoteThread(hp, 0, 0, (LPTHREAD_START_ROUTINE)(codeBuffer + remoteThreadStartOffset), codeBuffer, 0, 0);
    if (thread == nullptr)
    {
        LOGERROR("CreateRemoteThread fail. err:%d", GetLastError());
    }
    if (WaitForSingleObject(thread, 1000) != WAIT_OBJECT_0)
    {
        LOGERROR("remote thread error or timeout.");
        return false;
    }
    VirtualFreeEx(hp, codeBuffer, 0, MEM_RELEASE);
    return true;
}

bool open_and_inject_process(uint32_t pid, const wchar_t* dllName)
{
    auto process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (process == nullptr)
    {
        LOGERROR("Can't open process: %d, err:%d", pid, GetLastError());
        return false;
    }
    auto result = InjectRunningProcess(process, dllName);
    CloseHandle(process);
    return result;
}



}
