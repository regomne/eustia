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

static bool IsCmdOK(cmdline::parser& cmds)
{
    auto cmdType = cmds.get<string>("type");
    if ((cmdType == "start" && cmds.get<string>("file") == "") ||
        (cmdType == "open" && cmds.get<int>("pid") == 0) ||
        (cmdType == "hook" && (cmds.get<int>("pid") == 0 || cmds.get<string>("hook-type") == "")))
    {
        return false;
    }
    return true;
}

static int Main(cmdline::parser& cmds)
{
    GlobalVars::ModulePath = GetThisModulePath();
    if (!GlobalVars::ModulePath)
    {
        printf("Can't get module path.");
        return 0;
    }
    EustiaLogger::Init();
    EustiaLogger::Get()->SetLogPosition(EustiaLogger::LogPosStderr);
    EustiaLogger::Get()->SetLogLevel(EustiaLogger::LogLevelInfo);

    auto injectType = cmds.get<string>("type");
    if (injectType == "start")
    {
        
    }
    else if (injectType == "open")
    {
    }
    else if (injectType == "hook")
    {

    }

    EustiaLogger::Get()->Dispose();
    return 0;
}

int main(int argc, char* argv[])
{
    cmdline::parser cmd;
    cmd.add<string>("type", 't', "inject type (start, open or hook)", true, "",
        cmdline::oneof<string>("start", "open", "hook"));
    cmd.add<string>("file", 'f', "path of executable file (when start)", false, "");
    cmd.add<int>("pid", 'p', "process id (when open)", false, 0);
    cmd.add<string>("injectee", 'i', "path of injectee, dll or so", true, "");
    cmd.add<string>("hook-type", 'h', "Windows hook type (when hook)", false, "",
        cmdline::oneof<string>("keyboard", "msg", "callback"));
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
    if (cmds.get<string>("type") == "hook")
    {
        printf("hook type only supported on Windows!\n");
        return 0;
    }
#endif
    if (!IsCmdOK(cmd))
    {
        printf("some arguments not correct. Please read the doc.\n");
        return 0;
    }
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