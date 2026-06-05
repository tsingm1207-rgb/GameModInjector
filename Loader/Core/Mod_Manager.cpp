#include "Mod_Manager.h"

ModManager* g_mod_manager = nullptr;

typedef uint32_t (MODLOADER_CALL* ModEntryFunc)(mod_loader_api*, mod_exports*);
void MODLOADER_CALL ApiLog(uint32_t level, const std::string& msg)
{
    if(!g_mod_manager)
        return;
    g_mod_manager->GetCore().Log(level,msg);
}

void MODLOADER_CALL ApiSubscribeEvent(uint32_t event_type, mod_event_callback_fn callback, void* user_data)
{
    if(!g_mod_manager)
        return;
    g_mod_manager->GetCore().SubscribeEvent(event_type, callback, user_data);
}

bool MODLOADER_CALL ApiDrawText(mod_render_text_command* cmd){
    //g_mod_manager->GetCore().Log(MOD_LOG_DEBUG, "ApiDrawText");
    return g_mod_manager->GetCore().DrawTextCommand(cmd);
}

bool MODLOADER_CALL ApiDrawRect(mod_render_rect_command* cmd){
    //g_mod_manager->GetCore().Log(MOD_LOG_DEBUG, "ApiDrawRect");
    return g_mod_manager->GetCore().DrawRectCommand(cmd);
}

ModManager::ModManager(LoaderCore& core, std::filesystem::path mod_dir)
    : core_(core), mod_dir_(std::move(mod_dir))
{
    g_mod_manager = this;
    loader_api_.abi_version = MOD_ABI_VERSION;
    loader_api_.struct_size = sizeof(mod_loader_api);
    loader_api_.log = &ApiLog;
    loader_api_.subscribe_event = &ApiSubscribeEvent;
    loader_api_.draw_text = &ApiDrawText;
    loader_api_.draw_rect = &ApiDrawRect;
}

void ModManager::TickMods()
{
    for (auto& mod : loadedMods_)
    {
        if(mod.exports.on_tick)
        {
            mod.exports.on_tick();
        }
    }
}

void ModManager::UnloadMods()
{}

bool ModManager::LoadMods()
{
    core_.Log(MOD_LOG_INFO, "开始加载Mods，目录: " + mod_dir_.string());

    if (!std::filesystem::exists(mod_dir_) || !std::filesystem::is_directory(mod_dir_))
    {
        core_.Log(MOD_LOG_ERROR, "Mod目录不存在或不是一个目录: " + mod_dir_.string());
        return false;
    }

    int count = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(mod_dir_))
    {
        
        if (entry.is_regular_file() && entry.path().extension() == ".dll")
        {
            core_.Log(MOD_LOG_DEBUG, "发现文件: " + entry.path().string());
            count++;
            LoadSingleMod(entry.path());
        }
    }

    core_.Log(MOD_LOG_INFO, "Mods加载完成，共加载 " + std::to_string(count) + " 个dll");
    return true;
}


bool ModManager::LoadSingleMod(const std::filesystem::path& mod_path)
{
    core_.Log(MOD_LOG_INFO, "正在加载Mod: " + mod_path.string());
    HMODULE module = LoadLibraryW(mod_path.wstring().c_str());
    if (!module)
    {
        core_.Log(MOD_LOG_ERROR, "加载Mod失败: " + mod_path.string());
        return false;
    }

    auto entryFunc = (ModEntryFunc)GetProcAddress(module, "ModEntry");

    if(entryFunc == nullptr)
    {
        core_.Log(MOD_LOG_ERROR, "ModEntry函数未找到: " + mod_path.string());
        FreeLibrary(module);
        return false;
    }

    mod_exports exports{};
    exports.struct_size = sizeof(mod_exports);
    exports.abi_version = MOD_ABI_VERSION;

    uint32_t result = entryFunc(&loader_api_, &exports);
    if(result != 0)
    {
        core_.Log(MOD_LOG_ERROR, "ModEntry函数执行失败，错误码: " + std::to_string(result) + " Mod路径: " + mod_path.string());
        FreeLibrary(module);
        return false;
    }

    const size_t expected_size = sizeof(mod_exports);
    if(exports.abi_version != MOD_ABI_VERSION || exports.struct_size < expected_size)
    {
        core_.Log(MOD_LOG_ERROR, "Mod ABI版本不兼容: " + mod_path.string());
        FreeLibrary(module);
        return false;
    }

    LoadedMod mod{};
    mod.exports = exports;
    mod.fileName = mod_path.filename().string();
    mod.module = module;

    if(mod.exports.on_init)
    {
        mod.exports.on_init();
    }

    loadedMods_.push_back(std::move(mod));
    core_.Log(MOD_LOG_INFO, "成功加载Mod: " + mod_path.string());

    return true;
}