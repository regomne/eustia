#ifndef INJECTOR_H_
#define INJECTOR_H_

#include "src/common.h"

namespace EustiaInjector
{

bool CreateAndInject(Eustia::wchar* appName, const Eustia::wchar* dllName);
bool AdjustProcessTokenPrivilege();
bool OpenAndInjectProcess(Eustia::u32 pid, const Eustia::wchar* dllName);

}

#endif
