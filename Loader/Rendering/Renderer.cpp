#include <Windows.h>
#include "Renderer.h"
#include "../../Shared/mod_abi.h"

extern HMODULE g_hModule; 
void Renderer::Render(RenderQueue &renderQueue, HDC hdc)
{
    if(!hdc) return;

    if(!initialized_)
    {
        if(!Init(hdc)){
            logger_->Log(MOD_LOG_ERROR,"不能初始化渲染器");
            return;
        }

    } 
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    RenderQueued(renderQueue);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

bool Renderer::Init(HDC hdc)
{
    HWND hwnd = WindowFromDC(hdc);
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouse;

    // 加载中文字体
    HRSRC hRes = FindResourceA(g_hModule, "IDR_FONT_MSYH", (LPCSTR)RT_RCDATA);
    if (hRes)
    {
        HGLOBAL hData = LoadResource(g_hModule, hRes);
        if (hData)
        {
            DWORD size = SizeofResource(g_hModule, hRes);
            void* pData = LockResource(hData);

            ImFontConfig cfg;
            cfg.FontDataOwnedByAtlas = false; // 不释放资源段内存

            io.Fonts->AddFontFromMemoryTTF(
                pData,
                (int)size,
                18.f,
                &cfg,
                io.Fonts->GetGlyphRangesChineseSimplifiedCommon()
            );
            logger_->Log(MOD_LOG_INFO, "中文字体加载成功");
        }
        else
        {
            logger_->Log(MOD_LOG_ERROR, "LoadResource失败，使用默认字体");
            io.Fonts->AddFontDefault();
        }
    }
    else
    {
        logger_->Log(MOD_LOG_ERROR, "未找到字体资源IDR_FONT_MSYH，使用默认字体");
        io.Fonts->AddFontDefault();
    }

    if (!ImGui_ImplWin32_Init(hwnd))
    {
        ImGui::DestroyContext();
        logger_->Log(MOD_LOG_ERROR, "ImGui不能初始化Win32");
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init())
    {
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        logger_->Log(MOD_LOG_ERROR, "ImGui不能初始化OpenGL");
        return false;
    }

    initialized_ = true;
    logger_->Log(MOD_LOG_INFO, "渲染器初始化成功");
    return true;
}
void Renderer::RenderQueued(RenderQueue &renderQueue)
{
    auto packets = renderQueue.Drain();
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    for(auto &p : packets)
    {
        if(p.type ==MOD_RENDER_TEXT)
        {
            mod_render_text_command cmd{};
            std::memcpy(&cmd,p.payload.data(),sizeof(cmd));
            cmd.text[sizeof(cmd.text)-1] = '\0';
            drawList->AddText(ImVec2(cmd.x,cmd.y),ToColor(cmd.r,cmd.g,cmd.b,cmd.a),cmd.text);
        }
        else if(p.type == MOD_RENDER_RECT)
        {
            mod_render_rect_command cmd{};
            std::memcpy(&cmd,p.payload.data(),sizeof(cmd));
            drawList->AddRect(ImVec2(cmd.x,cmd.y),ImVec2(cmd.x+cmd.w,cmd.y+cmd.h),ToColor(cmd.r,cmd.g,cmd.b,cmd.a), 0.0f,0,cmd.thickness);
        }
    }
}

void Renderer::Shutdown()
{
    if(!initialized_) return;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    initialized_ = false;
    logger_->Log(MOD_LOG_INFO,"渲染器关闭");
}
