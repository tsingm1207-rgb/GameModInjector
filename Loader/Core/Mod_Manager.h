#pragma once
#include <Windows.h>
#include <filesystem>
#include "LoaderCore.h"
#include "../../Shared/mod_abi.h"

class LoaderCore;
class ModManager
{
public:
    ModManager(LoaderCore& core,std::filesystem::path mod_dir);
    
    bool LoadMods();
    void TickMods();
    void UnloadMods();

    LoaderCore& GetCore() { return core_; }
private:
    // 这里可以添加管理mod的成员变量和方法，例如加载mod、卸载mod、获取mod列表等
    bool LoadSingleMod(const std::filesystem::path& modPath);

    struct LoadedMod{
        std::string fileName;
        HMODULE module;
        mod_exports exports{};
    };

    mod_loader_api loader_api_{};

    LoaderCore& core_;
    std::filesystem::path mod_dir_;
    std::vector<LoadedMod> loadedMods_;
};  