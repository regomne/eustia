#ifndef UTILS_H_
#define UTILS_H_

#include <windows.h>

#include "src/common.h"

namespace Eustia
{

wchar* GetModulePathWin(HMODULE mod);
wchar* GetThisModulePath();
intptr GetProcessIdentifer();

}

#endif