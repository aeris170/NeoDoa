#pragma once

#include <memory>

#include "TypedefsAndConstants.hpp"

struct Mouse {
	double PosX, PosY, ScrollX, ScrollY;
	int Buttons[MOUSE_BUTTON_LAST];
};

struct Keyboard {
	int Keys[KEY_LAST];
};

struct Input {
	Mouse Mouse;
	Keyboard Keyboard;

	int IsKeyPressed(int key);
	int IsKeyTyped(int key);
	int IsKeyReleased(int key);

	int IsMouseButtonPressed(int button);
	int IsMouseButtonReleased(int button);

	double GetMouseX();
	double GetMouseY();
	double GetMouseScrollX();
	double GetMouseScrollY();
};

std::unique_ptr<Input> CreateInput();
