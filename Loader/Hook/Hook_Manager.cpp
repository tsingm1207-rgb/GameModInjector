#include "Hook_Manager.h"
#include <MinHook.h>

bool HookManager::Init()
{
    // Initialize MinHook
    if (MH_Initialize() != MH_OK)
    {
        return false;
    }
    return true;
}

bool HookManager::CreateHook(void* target, void* hook, void** trampoline)
{
    // Create a hook for the target function
    if (MH_CreateHook(target, hook, trampoline) != MH_OK)
    {
        return false;
    }
    return true;
}

bool HookManager::EnableHook(void* target)
{
    // Enable the hook for the target function
    if (MH_EnableHook(target) != MH_OK)
    {
        return false;
    }
    return true;
}

bool HookManager::DisableHook(void* target)
{
    // Disable the hook for the target function
    if (MH_DisableHook(target) != MH_OK)
    {
        return false;
    }
    return true;
}
