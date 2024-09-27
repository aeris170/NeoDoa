#pragma once

#include <array>
#include <memory>

#include <Engine/Point.hpp>

struct IWindow;

enum class Key {
    // Special Keys
    Unknown,
    Space,

    // Alphabet
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    // Numbers
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

    // Function Keys
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25,

    // Special Characters
    Apostrophe, Comma, Minus, Period, Slash, Semicolon, Equal, LeftBracket, Backslash, RightBracket, GraveAccent,

    // Navigation
    ArrowUp, ArrowDown, ArrowLeft, ArrowRight,

    // Control Keys
    Escape, Enter, Tab, Backspace, Insert, Delete, Home, End, PageUp, PageDown,

    // Modifier Keys
    LeftShift, RightShift, LeftControl, RightControl, LeftAlt, RightAlt, LeftSuper, RightSuper,

    // System Keys
    CapsLock, ScrollLock, NumLock, PrintScreen, Pause,

    // Keypad
    Keypad0, Keypad1, Keypad2, Keypad3, Keypad4, Keypad5, Keypad6, Keypad7, Keypad8, Keypad9,
    KeypadDecimal, KeypadDivide, KeypadMultiply, KeypadSubtract, KeypadAdd, KeypadEnter, KeypadEqual,

    // Miscellaneous
    Menu, World1, World2,

    Key_Last
};

enum class MouseButton {
    Left,
    Right,
    Middle,
    Mouse4,
    Mouse5,
    Mouse6,
    Mouse7,
    Mouse8,
    MouseButton_Last
};

struct MouseScroll {
    double XOffset;
    double YOffset;
};

struct Input {

    explicit Input(IWindow& window) noexcept;

    bool IsKeyPressed(Key key) const noexcept;
    bool IsKeyDepressed(Key key) const noexcept;
    bool IsKeyReleased(Key key) const noexcept;

    bool IsMouseButtonPressed(MouseButton button) const noexcept;
    bool IsMouseButtonDepressed(MouseButton button) const noexcept;
    bool IsMouseButtonReleased(MouseButton button) const noexcept;

    PointDouble GetMousePos() const noexcept;
    PointDouble GetMouseScroll() const noexcept;

    double GetMousePosX() const noexcept;
    double GetMousePosY() const noexcept;
    double GetMouseScrollX() const noexcept;
    double GetMouseScrollY() const noexcept;

private:

    struct State {
        bool Pressed{};
        bool Depressed{};
        bool Released{};
    };

    void Step() noexcept;

    State& StateOf(Key key) noexcept;
    const State& StateOf(Key key) const noexcept;

    State& StateOf(MouseButton button) noexcept;
    const State& StateOf(MouseButton button) const noexcept;

    void HandleKeyPress(Key key) noexcept;
    void HandleKeyRelease(Key key) noexcept;
    void HandleMousePress(MouseButton mouseButton) noexcept;
    void HandleMouseRelease(MouseButton mouseButton) noexcept;
    void HandleMouseMove(PointDouble position) noexcept;
    void HandleMouseScroll(MouseScroll scroll) noexcept;

    std::array<State, static_cast<size_t>(Key::Key_Last)> keyStates{};
    std::array<State, static_cast<size_t>(MouseButton::MouseButton_Last)> mouseButtonStates{};
    PointDouble mousePosition{};
    PointDouble mouseScroll{};

    friend struct Core;
};