#pragma once

struct GUI;

struct GameViewport {

	GUI* const gui;

	GameViewport(GUI* gui) noexcept;

	void Begin();
	void Render();
	void End();

};
