#pragma once

#include <functional>

#include <imgui.h>

#include <Engine/Log.hpp>

struct GUI;

struct Console {

    static constexpr auto TRACE_TOOLTIP_MESSAGE{ "General purpose hints/clues" };
    static constexpr auto INFO_TOOLTIP_MESSAGE{ "General notices" };
    static constexpr auto WARNING_TOOLTIP_MESSAGE{ "Explanations to potentially unwanted situations" };
    static constexpr auto ERROR_TOOLTIP_MESSAGE{ "Expected(!), but potentially very fragile situations. These will crash your game if you ship it with these :)" };
    static constexpr auto FATAL_TOOLTIP_MESSAGE{ "Unexpected behavior" };
    static constexpr auto OPENGL_TOOLTIP_MESSAGE{ "OpenGL Server message" };
    static constexpr auto VULKAN_TOOLTIP_MESSAGE{ "Vulkan Server message" };
    static constexpr auto DIRECTX_TOOLTIP_MESSAGE{ "Direct-X Server message" };

    static constexpr auto TRACE_BUTTON_TOOLTIP_MESSAGE{ "Show all messages" };
    static constexpr auto INFO_BUTTON_TOOLTIP_MESSAGE{ "Show only messages with severity INFO or higher" };
    static constexpr auto WARNING_BUTTON_TOOLTIP_MESSAGE{ "Show only messages with severity WARNING or higher" };
    static constexpr auto ERROR_BUTTON_TOOLTIP_MESSAGE{ "Show only ERROR and FATAL messages" };
    static constexpr auto FATAL_BUTTON_TOOLTIP_MESSAGE{ "Show only FATAL messages" };

    static constexpr auto CLEAR_BUTTON_TEXT{ "Clear" };
    static constexpr auto CLEAR_BUTTON_TOOLTIP_TEXT{ "Clear the console" };

    std::reference_wrapper<GUI> gui;

    explicit Console(GUI& gui) noexcept;

    bool Begin();
    void Render();
    void End();

private:
    float lineHeight{ 0 };
    ImVec2 buttonSize{ 0, 0 };

    LogSeverity selectedSeverity{ LogSeverity::TRACE };

    int oldCount{ 0 };

    void RenderTopPanel();
    void RenderFilterButtons();
    void RenderDummyArea() const;
    void RenderClearButton();

    void RenderMessageLog();
};
