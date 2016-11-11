#ifndef UTILS_H_
#define UTILS_H_

#include <windows.h>

#include "src/common.h"

namespace eustia
{

wchar* get_module_path_win(HMODULE mod);
wchar* get_this_module_path();
intptr get_process_identifier();

}

#endif