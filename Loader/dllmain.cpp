#include <Windows.h>
#include "Core/LoaderCore.h"

HMODULE g_hModule = nullptr;

DWORD WINAPI MainThread(LPVOID lpParam)
{
    static LoaderCore core;
    core.Initialize();
    return 0;
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            g_hModule = hinstDLL;
            DisableThreadLibraryCalls(hinstDLL);
            CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}