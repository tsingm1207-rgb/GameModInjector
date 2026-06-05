#pragma once
#include "RenderQueue.h"
#include "../Utils/Logger.h"
#include <wtypes.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_win32.h>


class Renderer
{
public:
    Renderer(Logger* logger) : logger_(logger) {};
    ~Renderer() {Shutdown();};

    void Render(RenderQueue& renderQueue,HDC hdc);

private:
    bool Init(HDC hdc);
    void RenderQueued(RenderQueue& renderQueue);
    void Shutdown();

    static ImU32 ToColor(float r, float g, float b, float a)
    {
        return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
    }
    Logger* logger_ = nullptr;
    bool initialized_ = false;
    HWND hwnd_ = nullptr;
};