#ifndef HOOKPROC_H_
#define HOOKPROC_H_

#include <windows.h>

#include "src/common.h"

EUSTIA_DLLAPI LRESULT WINAPI CallWndProc(int code, WPARAM wparam, LPARAM lparam);

#endif
