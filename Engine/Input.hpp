#pragma once

#include <array>
#include <memory>

#include "TypedefsAndConstants.hpp"

struct Input {
    int IsKeyPressed(int key) const noexcept;
    int IsKeyTyped(int key) const noexcept;
    int IsKeyReleased(int key) const noexcept;

    int IsMouseButtonPressed(int button) const noexcept;
    int IsMouseButtonReleased(int button) const noexcept;

    double GetMouseX() const noexcept;
    double GetMouseY() const noexcept;
    double GetMouseScrollX() const noexcept;
    double GetMouseScrollY() const noexcept;

private:
    struct Mouse {
        double PosX, PosY, ScrollX, ScrollY;
        std::array<int, MOUSE_BUTTON_LAST> Buttons;
    };

    struct Keyboard {
        std::array<int, KEY_LAST> Keys;
    };

    Mouse mouse;
    Keyboard keyboard;

    friend struct Window;
};

std::unique_ptr<Input> CreateInput();
