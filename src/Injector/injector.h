#ifndef INJECTOR_H_
#define INJECTOR_H_

#include "src/common.h"

namespace injector
{

bool create_and_inject(eustia::wchar* appName, const eustia::wchar* dllName);
bool adjust_process_token_privilege();
bool open_and_inject_process(eustia::u32 pid, const eustia::wchar* dllName);

}

#endif
