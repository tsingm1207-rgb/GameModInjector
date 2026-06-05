#pragma once
#include "Hook_Manager.h"
#include "../Core/LoaderCore.h"

namespace Hook
{
    bool InstallSwapBuffersHook(LoaderCore& core,HookManager& hookManager);
}