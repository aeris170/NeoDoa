#include "Input.hpp"

#include "Core.hpp"
#include "Window.hpp"

int IsKeyPressed(int key) {
	return GetCore()->_window->_keyboard._keys[key];
}

int IsKeyTyped(int key) {
	return GetCore()->_window->_keyboard._keys[key] == 1;
}

int IsKeyReleased(int key) {
	return compl GetCore()->_window->_keyboard._keys[key];
}

int IsMouseButtonPressed(int button) {
	return GetCore()->_window->_mouse._buttons[button];
}

int IsMouseButtonReleased(int button) {
	return compl GetCore()->_window->_mouse._buttons[button];
}

double GetMouseX() {
	return GetCore()->_window->_mouse._posx;
}

double GetMouseY() {
	return GetCore()->_window->_mouse._posy;
}

double GetMouseScrollX() {
	return GetCore()->_window->_mouse._scrollx;
}

double GetMouseScrollY() {
	return GetCore()->_window->_mouse._scrolly;
}