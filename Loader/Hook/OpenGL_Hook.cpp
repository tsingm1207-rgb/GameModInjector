#include "OpenGL_Hook.h"
#include <Windows.h>

// Function pointer for the original SwapBuffers function
typedef BOOL(WINAPI* wglSwapBuffersFunc)(HDC hdc);
wglSwapBuffersFunc originalSwapBuffers = nullptr;

LoaderCore *g_core = nullptr;
BOOL WINAPI HookedSwapBuffers(HDC hdc)
{
    if(g_core)
    {
        g_core->Tick(hdc);
    }
    // Call the original SwapBuffers function
    BOOL result = originalSwapBuffers(hdc);

    // Here you can add your custom code to be executed after SwapBuffers is called
    // For example, you could read pixels from the back buffer, apply post-processing effects, etc.

    return result;
}

bool Hook::InstallSwapBuffersHook(LoaderCore &core, HookManager &hookManager)
{
    HMODULE hOpenGL = GetModuleHandleW(L"opengl32.dll");
    if (!hOpenGL)    {
        hOpenGL = LoadLibraryW(L"opengl32.dll");
        if (!hOpenGL) {
            return false;
        }
    }
    g_core = &core;
    auto swapBuffersFunc = GetProcAddress(hOpenGL, "wglSwapBuffers");
    if (!swapBuffersFunc) {
        return false;
    }

    if(!hookManager.CreateHook(swapBuffersFunc, HookedSwapBuffers, (void**)&originalSwapBuffers))
    {
        FreeLibrary(hOpenGL);
        return false;
    }
    if(!hookManager.EnableHook(swapBuffersFunc))
    {
        hookManager.DisableHook(swapBuffersFunc);
        FreeLibrary(hOpenGL);
        return false;
    }

    return true;
}
