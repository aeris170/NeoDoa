#pragma once

#include <any>
#include <functional>
#include <string_view>

#include <imgui.h>
#include <imgui_internal.h>

struct KeyboardShortcutHandler {

    /*
        Key combos are for example: Ctrl+A or Ctrl+Shift+S. One or more modifier keys + at least one actual key.
    */
    void RegisterShortcut(std::string_view keyCombo, std::function<void()> onKeyComboEntered, ImGuiInputFlags flags = 0) noexcept;
    void RegisterShortcut(std::string_view keyCombo, std::function<void(std::any&)> onKeyComboEntered, std::any userData = {}, ImGuiInputFlags flags = 0) noexcept;

    void CheckShortcuts() noexcept;

private:
    struct Shortcut{
        ImGuiKeyChord chord;
        std::function<void(std::any&)> function;
        std::any userData{};
        ImGuiInputFlags flags;
    };
    std::vector<Shortcut> shortcuts{};
};