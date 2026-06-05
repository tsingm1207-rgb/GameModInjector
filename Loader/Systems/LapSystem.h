#pragma once
#include "../Core/LoaderCore.h"
class LoaderCore;
class LapSystem
{
public:
    LapSystem(LoaderCore* core);

    bool Init(uint32_t lapCompleteFnOffset,HookManager& hm);
};