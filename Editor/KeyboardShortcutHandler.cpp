#include <Editor/KeyboardShortcutHandler.hpp>

#include <Utility/Split.hpp>

void KeyboardShortcutHandler::RegisterShortcut(std::string_view keyCombo, std::function<void()> onKeyComboEntered, ImGuiInputFlags flags) noexcept {
    RegisterShortcut(keyCombo, [onKeyComboEntered](const std::any&) { onKeyComboEntered(); }, {}, flags);
}

void KeyboardShortcutHandler::RegisterShortcut(std::string_view keyCombo, std::function<void(std::any&)> onKeyComboEntered, std::any userData, ImGuiInputFlags flags) noexcept {
    std::string combo(keyCombo);
    auto tokens = split(combo, "+");

    ImGuiKeyChord chord{};
    for(const auto& token : tokens) {
        if (token.size() == 1) {
            int t = std::tolower(token.c_str()[0]);
            chord = chord | (t + 449);
        } else {
            if (token == "Ctrl") {
                chord = chord | ImGuiMod_Ctrl;
            }
            if (token == "Shift") {
                chord = chord | ImGuiMod_Shift;
            }
            if (token == "Alt") {
                chord = chord | ImGuiMod_Alt;
            }
        }
    }

    shortcuts.emplace_back(chord, onKeyComboEntered, userData, flags);
}

void KeyboardShortcutHandler::CheckShortcuts() noexcept {
    for (auto& shortcut : shortcuts) {
        if (ImGui::Shortcut(shortcut.chord, 0, shortcut.flags)) {
            shortcut.function(std::ref(shortcut.userData));
        }
    }
}
