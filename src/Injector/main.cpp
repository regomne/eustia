#include <windows.h>
#include <string>

#include "src/common.h"
#include "src/log.h"
#include "src/utils.h"
#include "src/localization.h"
#include "src/Injector/injector.h"
#include "src/Injector/globalvars_i.h"
#include "third_party/cmdline/cmdline.h"

using namespace std;
using namespace eustia;
using namespace injector;

static bool is_cmd_ok(cmdline::parser& cmds)
{
    auto cmd_type = cmds.get<string>("type");
    if ((cmd_type == "start" && cmds.get<string>("file") == "") ||
        (cmd_type == "open" && cmds.get<int>("pid") == 0) ||
        (cmd_type == "hook" && (cmds.get<int>("pid") == 0 || cmds.get<string>("hook-type") == "")))
    {
        return false;
    }
    return true;
}

static int main2(cmdline::parser& cmds)
{
    auto mod_path = get_this_module_path();
    if (mod_path == "")
    {
        printf("Can't get module path.\n");
        return 0;
    }
    GlobalVars::get()->module_path = mod_path;

    auto inject_type = cmds.get<string>("type");
    if (inject_type == "start")
    {
        auto file = cmds.get<string>("file");
        auto params = cmds.get<string>("file-param");
        auto mod = cmds.get<string>("injectee");
        if (!create_and_inject(file, params, mod))
        {
            printf("inject failed\n");
        }
        else
        {
            printf("inject success.\n");
        }
    }
    else if (inject_type == "open")
    {
        auto pid = cmds.get<int>("pid");
        auto mod = cmds.get<string>("injectee");
        if (!open_and_inject_process(pid, mod))
        {
            printf("open failed.\n");
        }
        else
        {
            printf("open success.\n");
        }
    }
    else if (inject_type == "hook")
    {

    }

    EustiaLogger::get()->dispose();
    return 0;
}

void init_cmdline(cmdline::parser& cmd)
{
#ifdef __X86
#define LOADER_DLL_NAME "loader32.dll"
#else
#define LOADER_DLL_NAME "loader64.dll"
#endif
    cmd.add<string>("type", 't', "inject type (start, open or hook)", true, "",
        cmdline::oneof<string>("start", "open", "hook"));
    cmd.add<string>("file", 'f', "path of executable file (when start)", false, "");
    cmd.add<string>("file-param", '\0', "parameters of executable (when start)", false, "");
    cmd.add<int>("pid", 'p', "process id (when open)", false, 0);
    cmd.add<string>("injectee", 'i', "path of injectee, dll or so", true, "");
    cmd.add<string>("loader-dll-name", '\0', "name of the loader dll", false, LOADER_DLL_NAME);
    cmd.add<string>("hook-type", 'h', "Windows hook type (when hook)", false, "",
        cmdline::oneof<string>("keyboard", "msg", "callback"));
    cmd.add("no-eustia", '\0', "only inject the module");
}

int main(int argc, char* argv[])
{
    EustiaLogger::init();
    GlobalVars::init();
    EustiaLogger::get()->set_log_position(EustiaLogger::LogPosStderr);
    EustiaLogger::get()->set_log_level(EustiaLogger::LogLevelInfo);

    cmdline::parser cmd;
    init_cmdline(cmd);

#ifdef _WINDOWS
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);
    auto cmd_string = GetCommandLine();
    auto cmd_utf8 = wchars_to_utf8(cmd_string);
    if (!cmd.parse(cmd_utf8))
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
    if (!is_cmd_ok(cmd))
    {
        printf("some arguments not correct. Please read the doc.\n");
        return 0;
    }
    return main2(cmd);
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