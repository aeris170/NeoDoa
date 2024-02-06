#include "Input.hpp"

std::unique_ptr<Input> CreateInput() {
    return std::make_unique<Input>();
}

int Input::IsKeyPressed(int key) const noexcept { return keyboard.Keys[key]; }
int Input::IsKeyTyped(int key) const noexcept { return keyboard.Keys[key] == 1; }
int Input::IsKeyReleased(int key) const noexcept { return !keyboard.Keys[key]; }

int Input::IsMouseButtonPressed(int button) const noexcept { return mouse.Buttons[button]; }
int Input::IsMouseButtonReleased(int button) const noexcept { return !mouse.Buttons[button]; }

double Input::GetMouseX() const noexcept { return mouse.PosX; }
double Input::GetMouseY() const noexcept { return mouse.PosY; }

double Input::GetMouseScrollX() const noexcept { return mouse.ScrollX; }
double Input::GetMouseScrollY() const noexcept { return mouse.ScrollY; }