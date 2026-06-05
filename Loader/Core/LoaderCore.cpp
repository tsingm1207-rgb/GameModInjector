#include "LoaderCore.h"
#include "../../Shared/mod_abi.h"
#include "../Hook/OpenGL_Hook.h"

static std::filesystem::path mods_dir("C:\\Users\\21585\\Desktop\\rs\\ModLoader\\build\\Mods");
void LoaderCore::Initialize()
{
    logger_ = std::make_unique<Logger>();
    if (!logger_->Init("C:\\Users\\21585\\Desktop\\rs\\ModLoader\\loader.log"))
    {
        return;
    }
    logger_->Log(MOD_LOG_INFO, "Logger已经初始化。");

    event_bus_ = std::make_unique<EventBus>();
    event_queue_ = std::make_unique<EventQueue>();
    renderer_ = std::make_unique<Renderer>(logger_.get());

    
    mod_manager_ = std::make_unique<ModManager>(*this, mods_dir);

    if(!mod_manager_->LoadMods())
    {
        logger_->Log(MOD_LOG_ERROR, "加载Mods失败！");
        logger_->Shutdown();
        return;
    }

    hook_manager_ = std::make_unique<HookManager>();

    if (!hook_manager_->Init())
    {
        logger_->Log(MOD_LOG_ERROR, "HookManager初始化失败！");
    }
    else
    {
        // Install the SwapBuffers hook
        if (!Hook::InstallSwapBuffersHook(*this, *hook_manager_))
        {
            logger_->Log(MOD_LOG_ERROR, "SwapBuffers Hook安装失败！");
        }
        else
        {
            logger_->Log(MOD_LOG_INFO, "SwapBuffers Hook安装成功！");
        }
    }


    lap_system_ = std::make_unique<LapSystem>(this);
    if(!lap_system_->Init(0x285910,*hook_manager_))
    {
        logger_->Log(MOD_LOG_ERROR, "LapSystem初始化失败！");
    }
        // Initialize the hook manager
    


}

void LoaderCore::Tick(HDC hdc)
{
    event_queue_->EnqueueEvent(MOD_EVENT_TICK, nullptr, 0);

    auto events = event_queue_->Drain();
    for (const auto& event : events)
    {
        const void* data = event.payload.empty() ? nullptr : event.payload.data();
        event_bus_->Dispatch(event.event_type, data, event.payload.size());
    }

    if(mod_manager_)
    {
        mod_manager_->TickMods();
    }

    renderer_->Render(render_queue_,hdc);
}

void LoaderCore::Log(uint32_t level, const std::string& msg)
{
    if (logger_)
    {
        logger_->Log(level, msg);
    }

}

void LoaderCore::SubscribeEvent(uint32_t event_type, mod_event_callback_fn callback, void *user_data)
{
    event_bus_->Subscribe(event_type, callback, user_data);
}

void LoaderCore::EnqueueEvent(uint32_t event_type, void *data, uint32_t size)
{
    event_queue_->EnqueueEvent(event_type, data, size);
}

bool LoaderCore::DrawTextCommand(mod_render_text_command *command)
{
    return render_queue_.Enqueue(MOD_RENDER_TEXT, command,sizeof(*command));
}

bool LoaderCore::DrawRectCommand(mod_render_rect_command *command)
{
    return render_queue_.Enqueue(MOD_RENDER_RECT, command, sizeof(*command));
}
