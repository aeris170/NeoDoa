#pragma once

#include <imgui.h>

#include "TypedefsAndConstants.hpp"

struct GLFWwindow;
struct ImGuiContext;

inline const ImVec4 WINDOW_BG{ 0.1f, 0.105f, 0.11f, 1.0f };

inline const ImVec4 IDLE{ 0.2f, 0.205f, 0.21f, 1.0f };
inline const ImVec4 HOVER{ 0.3f, 0.305f, 0.31f, 1.0f };
inline const ImVec4 ACTIVE{ 0.15f, 0.1505f, 0.151f, 1.0f };

inline const ImVec4 TAB_IDLE{ 0.15f, 0.1505f, 0.151f, 1.0f };
inline const ImVec4 TAB_HOVER{ 0.38f, 0.3805f, 0.381f, 1.0f };
inline const ImVec4 TAB_ACTIVE{ 0.28f, 0.2805f, 0.281f, 1.0f };
inline const ImVec4 TAB_ACTIVE_NO_FOCUS{ 0.2f, 0.205f, 0.21f, 1.0f };

inline const ImVec4 TITLE{ 0.15f, 0.1505f, 0.151f, 1.0f };

inline const ImWchar ICONS_RANGES_TURKISH[] = {
    0x0020, 0x00FF, // Basic Latin + Latin Supplement
    0x00c7, 0x00c7, // Ç
    0x00e7, 0x00e7, // ç
    0x011e, 0x011e, // Ð
    0x011f, 0x011f, // ð
    0x0130, 0x0130, // Ý
    0x0131, 0x0131, // ý
    0x00d6, 0x00d6, // Ö
    0x00f6, 0x00f6, // ö
    0x015e, 0x015e, // Þ
    0x015f, 0x015f, // þ
    0x00dc, 0x00dc, // Ü
    0x00fc, 0x00fc, // ü
    0
};

ImGuiContext* ImGuiInit(GLFWwindow* window);

void ImGuiRender(float delta);
void ImGuiAddRenderCommand(ImGuiFunction function);

void ImGuiClean();
