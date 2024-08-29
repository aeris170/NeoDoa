#include <Engine/Input.hpp>

#include <algorithm>

#include <Engine/Window.hpp>

Input::Input(IWindow& window) noexcept {
    window.Events.OnKeyPress   += std::bind_front(&Input::HandleKeyPress,   this);
    window.Events.OnKeyRelease += std::bind_front(&Input::HandleKeyRelease, this);

    window.Events.OnMousePress   += std::bind_front(&Input::HandleMousePress,   this);
    window.Events.OnMouseRelease += std::bind_front(&Input::HandleMouseRelease, this);

    window.Events.OnMouseMove   += std::bind_front(&Input::HandleMouseMove,   this);
    window.Events.OnMouseScroll += std::bind_front(&Input::HandleMouseScroll, this);
}

bool Input::IsKeyPressed(Key key)   const noexcept { return StateOf(key).Pressed;   }
bool Input::IsKeyDepressed(Key key) const noexcept { return StateOf(key).Depressed; }
bool Input::IsKeyReleased(Key key)  const noexcept { return StateOf(key).Released;  }

bool Input::IsMouseButtonPressed(MouseButton button)   const noexcept { return StateOf(button).Pressed;   }
bool Input::IsMouseButtonDepressed(MouseButton button) const noexcept { return StateOf(button).Depressed; }
bool Input::IsMouseButtonReleased(MouseButton button)  const noexcept { return StateOf(button).Released;  }

PointDouble Input::GetMousePos() const noexcept { return mousePosition; }
PointDouble Input::GetMouseScroll() const noexcept { return mouseScroll; }

double Input::GetMousePosX() const noexcept { return GetMousePos().X; }
double Input::GetMousePosY() const noexcept { return GetMousePos().Y; }
double Input::GetMouseScrollX() const noexcept { return GetMouseScroll().X; }
double Input::GetMouseScrollY() const noexcept { return GetMouseScroll().Y; }

void Input::Step() noexcept {
    std::ranges::for_each(keyStates, [](State& state) {
        if (state.Released) { state.Depressed = false; }
        state.Released = false;
        if (state.Pressed) { state.Depressed = true; }
        state.Pressed = false;
    });
    std::ranges::for_each(mouseButtonStates, [](State& state) {
        if (state.Released) { state.Depressed = false; }
        state.Released = false;
        if (state.Pressed) { state.Depressed = true; }
        state.Pressed = false;
    });
}

Input::State& Input::StateOf(Key key) noexcept {
    return keyStates[static_cast<decltype(std::to_underlying(key))>(key)];
}
const Input::State& Input::StateOf(Key key) const noexcept {
    return keyStates[static_cast<decltype(std::to_underlying(key))>(key)];
}

Input::State& Input::StateOf(MouseButton button) noexcept {
    return mouseButtonStates[static_cast<decltype(std::to_underlying(button))>(button)];
}
const Input::State& Input::StateOf(MouseButton button) const noexcept {
    return mouseButtonStates[static_cast<decltype(std::to_underlying(button))>(button)];
}

void Input::HandleKeyPress(Key key) noexcept { StateOf(key).Pressed = true; }
void Input::HandleKeyRelease(Key key) noexcept {
    State& state = StateOf(key);
    state.Depressed = true;
    state.Released = true;
}
void Input::HandleMousePress(MouseButton mouseButton) noexcept { StateOf(mouseButton).Pressed = true; }
void Input::HandleMouseRelease(MouseButton mouseButton) noexcept {
    State& state = StateOf(mouseButton);
    state.Depressed = true;
    state.Released = true;
}
void Input::HandleMouseMove(PointDouble position) noexcept { mousePosition = position; }
void Input::HandleMouseScroll(MouseScroll scroll) noexcept { mouseScroll = { scroll.XOffset, scroll.YOffset }; }