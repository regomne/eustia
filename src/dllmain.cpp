#include <windows.h>

int WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
    UNREFERENCED_PARAMETER(_DllHandle);
    UNREFERENCED_PARAMETER(_Reserved);

    switch (_Reason)
    {
    case DLL_PROCESS_ATTACH:
        MessageBox(0, L"dll loaded", 0, 0);
        break;
    default:
        break;
    }

    return 0;
}