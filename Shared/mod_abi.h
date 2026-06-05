#pragma once
#include <cstdint>
#include <string>

#define MOD_ABI_VERSION 1u

#define MODLOADER_CALL __stdcall
#define MODLOADER_EXPORT __declspec(dllexport)

typedef void (MODLOADER_CALL* mod_event_callback_fn)(uint32_t event_type, const void* event_data,uint32_t event_size,void* user_data);

enum mod_log_level: uint32_t
{
    MOD_LOG_DEBUG = 0,
    MOD_LOG_INFO = 1,
    MOD_LOG_ERROR = 2
};

enum mod_event_type: uint32_t
{
    MOD_EVENT_TICK = 1,
    MOD_EVENT_LAP_COMPLETE = 2
};

struct mod_lap_complete_event
{
    uintptr_t arg1;
    uint32_t arg2;
};

enum mod_render_command_type  : uint32_t
{
    MOD_RENDER_TEXT = 1,
    MOD_RENDER_RECT = 2
};

struct mod_render_text_command
{
    float x,y;
    float r,g,b,a;
    char text[512];
};

struct mod_render_rect_command
{
    float x,y,w,h;
    float thickness;
    float r,g,b,a;
};

struct mod_loader_api
{
    uint32_t abi_version;
    uint32_t struct_size;

    void (MODLOADER_CALL* log)(uint32_t level, const std::string& msg);
    void (MODLOADER_CALL* subscribe_event)(uint32_t event_type, mod_event_callback_fn callback, void* user_data);

    bool (MODLOADER_CALL* draw_text)(mod_render_text_command* cmd);
    bool (MODLOADER_CALL* draw_rect)(mod_render_rect_command* cmd);
};

struct mod_exports
{
    uint32_t abi_version;
    uint32_t struct_size;

    void (MODLOADER_CALL* on_init)();
    void (MODLOADER_CALL* on_tick)();
};

extern "C" MODLOADER_EXPORT uint32_t MODLOADER_CALL ModEntry(mod_loader_api* api, mod_exports* out_exports);