#pragma once

class HookManager
{
public:
    bool Init();

    bool CreateHook(void* target, void* hook, void** trampoline);
    bool EnableHook(void* target);
    bool DisableHook(void* target);

private:

};