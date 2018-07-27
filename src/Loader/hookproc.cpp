#include "src/HookDll/hookproc.h"

LRESULT WINAPI CallWndProc(int code, WPARAM wparam, LPARAM lparam)
{

    return CallNextHookEx(0, code, wparam, lparam);
}