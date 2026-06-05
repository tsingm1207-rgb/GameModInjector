#include "LapSystem.h"

typedef void (__stdcall* LapCompleteFn)(uintptr_t arg1, uint32_t arg2);
LapCompleteFn orig_lapComplete = nullptr;

static LoaderCore *g_core = nullptr;
void __stdcall LapComplete_Hook(uintptr_t arg1, uint32_t arg2)
{
    //g_core->Log(MOD_LOG_INFO,"LapSystem LapComplete触发");
     if(g_core)
     {
        mod_lap_complete_event evt{};
        evt.arg1 = arg1;
        evt.arg2 = arg2;

        g_core->EnqueueEvent(MOD_EVENT_LAP_COMPLETE, &evt,sizeof(evt));
     }
    orig_lapComplete(arg1, arg2);
}
LapSystem::LapSystem(LoaderCore *core)
{
    g_core = core;
}

bool LapSystem::Init(uint32_t lapCompleteFnOffset, HookManager &hm)
{
    if(!g_core || lapCompleteFnOffset == 0) return false;

    HMODULE exeBase = GetModuleHandleA(nullptr);

    uintptr_t address = reinterpret_cast<uintptr_t>(exeBase) + lapCompleteFnOffset;

    void* target = reinterpret_cast<void*>(address);

    if(!hm.CreateHook(target, reinterpret_cast<void*>(&LapComplete_Hook), reinterpret_cast<void**>(&orig_lapComplete)))
    {
        return false;
    }

    if(!hm.EnableHook(target))
    {
        return false;
    }
    g_core->Log(MOD_LOG_INFO,"LapSystem LapComplete成功hook");
    return true;
}
