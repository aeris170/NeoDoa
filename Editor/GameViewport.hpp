#pragma once

#include <functional>

struct GUI;

struct GameViewport {

	std::reference_wrapper<GUI> gui;

	GameViewport(GUI& gui) noexcept;

	void Begin();
	void Render();
	void End();

};
