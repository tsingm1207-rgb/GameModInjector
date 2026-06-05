#pragma once
#include "../../Shared/mod_abi.h"

const static mod_loader_api* g_api = nullptr;

static uint32_t g_lastKartIndex = 0;
static bool g_hasLapInfo = false;

void OnTick(uint32_t event_type, const void* event_data,uint32_t event_size,void* user_data)
{
   // g_api->log(MOD_LOG_INFO, "TestMod Tick!");
    
    mod_render_text_command cmd{};
    cmd.x = 20.0f; cmd.y = 20.0f; 
    cmd.r = 1.0f; cmd.g = 1.0f; cmd.b = 1.0f; cmd.a = 1.0f;
   
    
    if(!g_hasLapInfo)
    {
         std::snprintf(cmd.text, sizeof(cmd.text), "还没有完成圈数...");
            
    }else
    {
        std::snprintf(cmd.text, sizeof(cmd.text), "已完成圈数: %u", g_lastKartIndex);
    }
    cmd.text[sizeof(cmd.text) - 1] = '\0';
    if(g_api->draw_text) 
    {
        g_api->draw_text(&cmd);
    }
    
    
}

void OnLapComplete(uint32_t event_type, const void* event_data,uint32_t event_size,void* user_data)
{
   
    const auto* evt = static_cast<const mod_lap_complete_event*>(event_data);
    
    g_lastKartIndex = evt->arg2;
    g_hasLapInfo = true;
}
void OnInit()
{
    if(g_api){
        g_api->log(MOD_LOG_INFO, "TestMod初始化完成！");
        g_api->subscribe_event(MOD_EVENT_TICK, OnTick, 0);
        g_api->subscribe_event(MOD_EVENT_LAP_COMPLETE, OnLapComplete, 0);
    }
    
}

extern "C" MODLOADER_EXPORT uint32_t MODLOADER_CALL ModEntry(mod_loader_api* api, mod_exports* out_exports)
{
    if (api->abi_version != MOD_ABI_VERSION || api->struct_size != sizeof(mod_loader_api))
    {
        return 1; // ABI版本不兼容
    }

    g_api = api;

    out_exports->abi_version = MOD_ABI_VERSION;
    out_exports->struct_size = sizeof(mod_exports);
    out_exports->on_init = &OnInit; // 这里可以设置mod的初始化函数

    return 0; // 成功
    
}