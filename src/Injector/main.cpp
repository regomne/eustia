#include <windows.h>
#include <string>

#include "src/common.h"
#include "src/log.h"
#include "src/utils.h"
#include "src/Injector/injector.h"
#include "src/Injector/globalvars_i.h"
#include "3rdParty/cmdline/cmdline.h"

using namespace std;
using namespace Eustia;
using namespace EustiaInjector;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    GlobalVars::ModulePath = GetModulePath(GetModuleHandle(nullptr));
    auto logPath = wstring(GlobalVars::ModulePath) + L"injector.log";
    EustiaLogger::GetLogger()->SetLogFileName(logPath.c_str());
    EustiaLogger::GetLogger()->SetCurrentLogLevel(EustiaLogger::LogLevelInfo);
    //auto cmd = GetCommandLine();
    //int cmdCnt;
    //auto cmds = CommandLineToArgvW(cmd, &cmdCnt);
//     if (cmds == nullptr || cmdCnt < 2)
//     {
//         LOGDEBUG("need 2 args.");
//         return 0;
//     }
    MessageBox(0, L"opening...", 0, 0);

    AdjustProcessTokenPrivilege();

    auto ns = wstring(GlobalVars::ModulePath) + L"eustia.dll";
//     if (!CreateAndInject(cmds[1], ns.c_str()))
//     {
//         MessageBox(0, L"open fail!", 0, 0);
//     }
    if (!OpenAndInjectProcess(9156, ns.c_str()))
    {
        MessageBox(0, L"open inject fail!", 0, 0);
    }

    return 0;
}