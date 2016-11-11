#ifndef INJECTOR_H_
#define INJECTOR_H_

#include "src/common.h"

namespace injector
{

bool create_and_inject(wchar_t* appName, const wchar_t* dllName);
bool adjust_process_token_privilege();
bool open_and_inject_process(uint32_t pid, const wchar_t* dllName);

}

#endif
