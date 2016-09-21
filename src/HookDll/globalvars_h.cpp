#include "src/HookDll/globalvars_h.h"

using namespace Eustia;

wchar* GlobalVars::ModulePath;
HANDLE GlobalVars::DllHandle;
bool GlobalVars::NeedDelayCheck;
IPCInfo GlobalVars::IpcInfo;
