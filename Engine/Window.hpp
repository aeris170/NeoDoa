#pragma once

#include <string>

#include "TypedefsAndConstants.hpp"
#include "Resolution.hpp"

struct GLFWwindow;
struct ImGuiContext;

struct Window {
	Resolution _resolution;
	Resolution _contentResolution;
	std::string _title;
	bool _isFullscreen;
	GLFWwindow* _glfwWindow;
	ImGuiContext* _imGuiContext;

	struct Mouse {
		double _posx, _posy, _scrollx, _scrolly;
		int _buttons[MOUSE_BUTTON_LAST];
	} _mouse;
	struct Keyboard {
		int _keys[KEY_LAST];
	} _keyboard;

	// don't call, this function should only be called by CreateCore
	Window(Resolution resolution, const char* title, bool isFullscreen, const char* windowIcon) noexcept;
	// don't call
	~Window() noexcept;
	Window(const Window&) = delete;
	Window(const Window&&) = delete;
	Window& operator=(const Window&) = delete;
	Window& operator=(const Window&&) = delete;

	std::string GetTitle() const;
	void SetTitle(const std::string& title);
	void SetTitle(std::string&& title);

	void DisableCursor();
	void HideCursor();
	void EnableCursor();

};
