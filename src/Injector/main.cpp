#include <windows.h>
#include <string>

#include "src/common.h"
#include "src/log.h"
#include "src/utils.h"
#include "src/Injector/injector.h"
#include "src/Injector/globalvars_i.h"
#include "3rdParty/cmdline/cmdline.h"
#include "3rdParty/LLVM/ConvertUTF8.h"

using namespace std;
using namespace Eustia;
using namespace EustiaInjector;

int Main(cmdline::parser& cmds)
{
    cmds;
    if (!GlobalVars::Init())
    {
        return -1;
    }
    auto logPath = wstring(GlobalVars::ModulePath) + L"injector.log";
    EustiaLogger::GetLogger()->SetLogFileName(logPath.c_str());
    EustiaLogger::GetLogger()->SetCurrentLogLevel(EustiaLogger::LogLevelInfo);

    return 0;
}

int main(int argc, char* argv[])
{
    cmdline::parser cmd;
    cmd.add<string>("type", 't', "inject type", true, "",
        cmdline::oneof<string>("start", "open", "hook"));
    cmd.add<string>("file", 'f', "path of executable file (when start)", false, "");
    cmd.add<int>("pid", 'p', "process id (when open)", false, 0);
    cmd.add<string>("injectee", 'i', "path of injectee, dll or so", true, "");
    cmd.add("no-eustia", '\0', "only inject the module");

#ifdef _WINDOWS
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);
    auto cmdString = GetCommandLine();
    auto cmdLen = WideCharToMultiByte(CP_UTF8, 0, cmdString, -1, 0, 0, 0, 0);
    auto cmdUtf8 = new char[cmdLen];
    WideCharToMultiByte(CP_UTF8, 0, cmdString, -1, cmdUtf8, cmdLen, 0, 0);
    if (!cmd.parse(cmdUtf8))
    {
        printf("%s\n", cmd.usage().c_str());
        printf("example: //todo\n");
        return 0;
    }
#else
    if (!cmd.parse(argc, argv))
    {
        printf("%s\n", cmd.usage().c_str());
        printf("example: //todo\n");
        return 0;
    }
#endif
    return Main(cmd);
}

//int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
//{
//    UNREFERENCED_PARAMETER(hInstance);
//    UNREFERENCED_PARAMETER(hPrevInstance);
//    UNREFERENCED_PARAMETER(lpCmdLine);
//    UNREFERENCED_PARAMETER(nShowCmd);
//
//
//    auto cmd = GetCommandLine();
//    int cmdCnt;
//    auto cmds = CommandLineToArgvW(cmd, &cmdCnt);
////     if (cmds == nullptr || cmdCnt < 2)
////     {
////         LOGDEBUG("need 2 args.");
////         return 0;
////     }
//    AdjustProcessTokenPrivilege();
//
//    auto ns = wstring(GlobalVars::ModulePath) + L"eustia.dll";
////     if (!CreateAndInject(cmds[1], ns.c_str()))
////     {
////         MessageBox(0, L"open fail!", 0, 0);
////     }
//    if (!OpenAndInjectProcess(9156, ns.c_str()))
//    {
//        MessageBox(0, L"open inject fail!", 0, 0);
//    }
//
//    return 0;
//}