#include "src/HookDll/globalvars_h.h"

using namespace eustia;

wchar_t* GlobalVars::ModulePath;
HANDLE GlobalVars::DllHandle;
bool GlobalVars::NeedDelayCheck;
IPCInfo GlobalVars::IpcInfo;
