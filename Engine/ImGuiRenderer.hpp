#pragma once

#include <functional>

#include <imgui.h>

#include <Engine/Window.hpp>

struct ImGuiContext;

using TextureHandle = ImTextureID;

using ImGuiFunction = std::function<void(float)>;

inline const ImVec4 WINDOW_BG{ 0.1f, 0.105f, 0.11f, 1.0f };
inline const ImVec4 WINDOW_BG_ALT{ 0.2f, 0.205f, 0.21f, 1.0f };

inline const ImVec4 IDLE{ 0.2f, 0.205f, 0.21f, 1.0f };
inline const ImVec4 HOVER{ 0.3f, 0.305f, 0.31f, 1.0f };
inline const ImVec4 ACTIVE{ 0.15f, 0.1505f, 0.151f, 1.0f };

inline const ImVec4 TAB_IDLE{ 0.15f, 0.1505f, 0.151f, 1.0f };
inline const ImVec4 TAB_HOVER{ 0.38f, 0.3805f, 0.381f, 1.0f };
inline const ImVec4 TAB_ACTIVE{ 0.28f, 0.2805f, 0.281f, 1.0f };
inline const ImVec4 TAB_ACTIVE_NO_FOCUS{ 0.2f, 0.205f, 0.21f, 1.0f };

inline const ImVec4 TITLE{ 0.15f, 0.1505f, 0.151f, 1.0f };

inline const ImVec4 CHECKMARK{ 0.2f, 0.702f, 0.2f, 1.0f };

inline const ImWchar ICONS_RANGES_TURKISH[] = {
    0x0020, 0x00FF, // Basic Latin + Latin Supplement
    0x00c7, 0x00c7, // �
    0x00e7, 0x00e7, // �
    0x011e, 0x011e, // �
    0x011f, 0x011f, // �
    0x0130, 0x0130, // �
    0x0131, 0x0131, // �
    0x00d6, 0x00d6, // �
    0x00f6, 0x00f6, // �
    0x015e, 0x015e, // �
    0x015f, 0x015f, // �
    0x00dc, 0x00dc, // �
    0x00fc, 0x00fc, // �
    0
};

ImGuiContext* ImGuiInit(IWindow& window);
void ImGuiSetUpWindowIcons(WindowIconPack icons);

void ImGuiRender(float delta);
void ImGuiAddRenderCommand(ImGuiFunction function);

void ImGuiClean();
