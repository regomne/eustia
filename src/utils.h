#ifndef UTILS_H_
#define UTILS_H_

#include <windows.h>

#include "src/common.h"

namespace eustia
{

wchar_t* get_module_path_win(HMODULE mod);
wchar_t* get_this_module_path();
intptr_t get_process_identifier();

}

#endif