#include "src/Injector/injector.h"

#include <windows.h>
#include <assert.h>

#include "src/common.h"
#include "src/log.h"

using namespace Eustia;


namespace EustiaInjector
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

u32 GetLoadLibLength(intptr* jmpPos)
{
    //assume length of Loadlib() less than 0x50 bytes
    for (auto p = (u8*)LoadLib;p < (u8*)LoadLib + 0x50;p++)
    {
        //search mov eax,0xffffffff
        if (*p == 0xb8 && *(u32*)(p + 1) == 0xffffffff)
        {
            *jmpPos = (intptr)(p - (u8*)LoadLib);
            return p + 5 - (u8*)LoadLib;
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

bool InjectStartingProcess(HANDLE hp, HANDLE ht, const wchar* dllPath)
{
    ProcInfo procInfo;
    intptr jmpPos;
    auto loadLibLength = GetLoadLibLength(&jmpPos);
    auto dllPathLength = wcslen(dllPath);
    auto loadLibStartOffset = sizeof(procInfo);
    auto dllPathStartOffset = loadLibStartOffset + loadLibLength;
    //align to 4 bytes
    dllPathStartOffset = (dllPathStartOffset & 3) ? ((dllPathStartOffset & ~3) + 4) : dllPathStartOffset;
    auto codeBuffer = (u8*)VirtualAllocEx(
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
    u8 jmpGates[5] = { 0xe9,0,0,0,0 };
    *(u32*)&jmpGates[1] = ctt.Eip - (u32)(codeBuffer + loadLibStartOffset + jmpPos + 5);

    SIZE_T bytesWrote = 0;
    bool wroteSuccess = true;
    wroteSuccess = wroteSuccess && WriteProcessMemory(hp, codeBuffer, &procInfo, sizeof(procInfo), &bytesWrote);
    wroteSuccess = wroteSuccess && WriteProcessMemory(hp, codeBuffer + loadLibStartOffset, &LoadLib, jmpPos, &bytesWrote);
    wroteSuccess = wroteSuccess && WriteProcessMemory(hp, codeBuffer + loadLibStartOffset + jmpPos, jmpGates, 5, &bytesWrote);
    wroteSuccess = wroteSuccess && WriteProcessMemory(hp, codeBuffer + dllPathStartOffset, dllPath, dllPathLength * sizeof(wchar), &bytesWrote);

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
bool CreateAndInject(wchar* appName, const wchar* dllPath)
{
    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    if (GetFileAttributes(appName) == INVALID_FILE_ATTRIBUTES)
    {
        LOGERROR("Can't find %s", LOGASTR(appName));
        return false;
    }

    if (!CreateProcess(0, appName, 0, 0, FALSE, CREATE_SUSPENDED, 0, 0, &si, &pi))
    {
        auto err = GetLastError();
        LOGERROR("Can't create process: %s, error:%d.", LOGASTR(appName), err);
        return false;
    }

    if (!InjectStartingProcess(pi.hProcess, pi.hThread, dllPath))
    {
        TerminateProcess(pi.hProcess, 0);
        return false;
    }

    ResumeThread(pi.hThread);

    return true;
}
#endif // __X86

bool AdjustProcessTokenPrivilege()
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

//assume this function's length max 200bytes.
static const size_t LenOfRemoteThreadProc = 208;
static DWORD WINAPI RemoteThreadProc(LPVOID param)
{
    auto procInfo = (ProcInfo*)param;
    procInfo->LdrLoadDllFunc(0, 0, &procInfo->dllName, &procInfo->dllHandle);
    return 0;
}

bool InjectRunningProcess(HANDLE hp, const wchar* dllName)
{
    ProcInfo procInfo;
    auto dllPathLength = wcslen(dllName);
    auto remoteThreadStartOffset = sizeof(procInfo);
    auto dllPathStartOffset = remoteThreadStartOffset + LenOfRemoteThreadProc;
    dllPathStartOffset = (dllPathStartOffset & 3) ? ((dllPathStartOffset & ~3) + 4) : dllPathStartOffset;
    auto codeBuffer = (u8*)VirtualAllocEx(
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
    wroteSuccess = wroteSuccess && WriteProcessMemory(hp, codeBuffer + dllPathStartOffset, dllName, dllPathLength * sizeof(wchar), &bytesWrote);
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

bool OpenAndInjectProcess(u32 pid, const wchar* dllName)
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

