#ifndef UTILS_H_
#define UTILS_H_

#ifdef _WINDOWS
#include <windows.h>
#endif

#include <string>
#include <cstdio>

#include "src/common.h"

namespace eustia
{

#ifdef _WINDOWS
std::string get_module_path_win(HMODULE mod);
#endif

std::string get_this_module_path();

intptr_t get_process_identifier();

std::FILE* open_file_utf8(const std::string& fname, const char* flags)

}

#endif