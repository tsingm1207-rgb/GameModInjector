#pragma once
#include <string>
#include <memory>
#include "../Utils/Logger.h"
#include "Mod_Manager.h"
#include "../Hook/Hook_Manager.h"
#include "Event_Bus.h"
#include "EventQueue.h"
#include "../Rendering/RenderQueue.h"
#include "../Rendering/Renderer.h"
#include "../Systems/LapSystem.h"

class ModManager;
class LapSystem;
class LoaderCore
{
public:
    void Initialize();
    void Tick(HDC hdc);
    void Log(uint32_t level, const std::string& msg);

    void SubscribeEvent(uint32_t event_type, mod_event_callback_fn callback, void* user_data);
    void EnqueueEvent(uint32_t event_type, void* data, uint32_t size);

    bool DrawTextCommand(mod_render_text_command* command);
    bool DrawRectCommand(mod_render_rect_command* command);
private:
    std::unique_ptr<Logger> logger_;
    std::unique_ptr<ModManager> mod_manager_;
    std::unique_ptr<HookManager> hook_manager_;
    std::unique_ptr<EventBus> event_bus_;
    std::unique_ptr<EventQueue> event_queue_;
    std::unique_ptr<Renderer> renderer_;

    RenderQueue render_queue_;

    std::unique_ptr<LapSystem> lap_system_;
};