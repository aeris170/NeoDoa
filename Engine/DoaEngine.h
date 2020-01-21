// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace doa {

	typedef GLFWwindow Window;

	extern DOA_API double UPS;
	extern DOA_API bool running;
	extern DOA_API int window_width;
	extern DOA_API int window_height;
	extern DOA_API glm::vec3 clear_color;

	DOA_API int Init();
	DOA_API Window* CreateWindow(const char* name, const int width, const int height, const bool isFullscreen = false);
	DOA_API void Loop();
	DOA_API void Purge();
}

namespace internal {

	typedef GLFWwindow Window;

	extern Window* m_window;
	extern glm::mat4 m_view_matrix;
	extern glm::mat4 m_fixed_view_matrix;
	extern glm::mat4 m_projection_matrix;

	void update();
	void render();
	void on_window_resize(Window* window, int width, int height);
	void on_mouse_button_click_and_release(Window* window, int button, int action, int mods);
	void on_mouse_move(Window* window, double xpos, double ypos);
	void on_mouse_scroll(Window* window, double xoffset, double yoffset);
	void on_key_press_and_release(Window* window, int key, int scancode, int action, int mods);
	void calculate_view_matrix();
	void calculate_projection_matrix();
}