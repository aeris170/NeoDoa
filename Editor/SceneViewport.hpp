#pragma once

struct GUI;

struct SceneViewport {

	GUI* const gui;

	SceneViewport(GUI* gui) noexcept;

	void Begin();
	void Render();
	void End();

};
