#include "Input.hpp"

std::unique_ptr<Input> CreateInput() {
    return std::make_unique<Input>();
}

int Input::IsKeyPressed(int key) { return Keyboard.Keys[key]; }
int Input::IsKeyTyped(int key) { return Keyboard.Keys[key] == 1; }
int Input::IsKeyReleased(int key) { return !Keyboard.Keys[key]; }

int Input::IsMouseButtonPressed(int button) { return Mouse.Buttons[button]; }
int Input::IsMouseButtonReleased(int button) { return !Mouse.Buttons[button]; }

double Input::GetMouseX() { return Mouse.PosX; }
double Input::GetMouseY() { return Mouse.PosY; }

double Input::GetMouseScrollX() { return Mouse.ScrollX; }
double Input::GetMouseScrollY() { return Mouse.ScrollY; }