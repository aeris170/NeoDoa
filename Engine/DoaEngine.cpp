// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa {
	using namespace internal;

	double UPS{ 240 };
	bool running{ false };
	int window_width{ 0 };
	int window_height{ 0 };
	glm::vec3 clear_color{ .07843f, .05843f, .07843f };

	int Init() {
		{ //GLFW INIT
			if (!glfwInit()) {
				std::cout << "Fucked up while initializing glfw!\n";
				glfwTerminate();
				return 1;
			}
		}
		return 0;
	}

	Window* CreateWindow(const char* name, const int width, const int height, const bool isFullscreen) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLU_TRUE);
		glfwWindowHint(GLFW_SAMPLES, 4);

		window_width = width;
		window_height = height;
		m_window = glfwCreateWindow(width, height, name, isFullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

		if (!m_window) {
			std::cout << "Fucked up while creating GLFWwindow!\n";
		}

		glViewport(0, 0, width, height);
		calculate_view_matrix();
		calculate_projection_matrix();
		glfwSetWindowSizeCallback(m_window, on_window_resize);
		glfwSetMouseButtonCallback(m_window, on_mouse_button_click_and_release);
		glfwSetCursorPosCallback(m_window, on_mouse_move);
		glfwSetScrollCallback(m_window, on_mouse_scroll);
		glfwSetKeyCallback(m_window, on_key_press_and_release);
		glfwMakeContextCurrent(m_window);

		{ //WINDOW ICON
			const int iconCount{ 8 };
			const std::string path{ "Icons/window/def_win_icon_" };
			const std::string ext{ ".png" };
			GLFWimage images[iconCount];
			for (int i = 0; i < 8; i++) {
				int size = (int)(2048 / pow(2, i));
				images[i].pixels = stbi_load((path + std::to_string(size) + ext).c_str(), &images[i].width, &images[i].height, 0, STBI_rgb_alpha);
			}
			glfwSetWindowIcon(m_window, 8, images);
			for (int i = 0; i < iconCount; i++) {
				stbi_image_free(images[i].pixels);
			}
		}

		{ //GLEW INIT
			glewExperimental = GL_TRUE;
			if (glewInit() != GLEW_OK) {
				std::cout << "Fucked up while initializing glew!\n";
				glfwDestroyWindow(m_window);
				glfwTerminate();
			}
			primitives::Primitive::generate_buffers();
			shader::CreateShaderProgram("primitives-shader", "Shaders/vShader", "Shaders/fShader");
			internal::ezrender::instantiate_primitives();
		}

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);

		return m_window;
	}

	void Loop() {
		running = true;

		double now{ 0 }, delta{ 0 };
		double last{ glfwGetTime() }, timer{ last };
		int updates{ 0 }, frames{ 0 };

		while (!glfwWindowShouldClose(m_window)) {
			glfwPollEvents();

			now = glfwGetTime();
			delta += (now - last) * UPS;
			last = now;

			while (delta >= 1.0) {
				update();
				++updates;
				--delta;
			}
			{   // - Render at maximum possible frames
				glClearColor(clear_color.r, clear_color.g, clear_color.b, 1);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				render();
				++frames;
			}
			// - Reset after one second
			if (glfwGetTime() - timer > 1) {
				++timer;
				#ifdef _DEBUG
					std::cout << "FPS: " << frames << " Updates:" << updates << '\n';
				#endif
				updates = 0, frames = 0;
			}

			glfwSwapBuffers(m_window);
		}

		running = false;
	}

	void Purge() {
		internal::animation::purge();
		internal::texture::purge();
		internal::scene::purge();
		internal::shader::purge();
		internal::ezrender::purge();
		primitives::Primitive::purge();
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}
}

namespace internal {

	Window* m_window{ NULL };

	glm::mat4 m_view_matrix;
	glm::mat4 m_fixed_view_matrix{ glm::lookAt(glm::vec3(0, 0, 5000), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)) };
	glm::mat4 m_projection_matrix;

	void update() {
		keyboard::update();
		mouse::update();
		if (doa::scene::ACTIVE_SCENE) {
			doa::scene::ACTIVE_SCENE->update();
		}
		camera::update();
		calculate_view_matrix();
		if (doa::keyboard::IsKeyHold(doa::keyboard::W)) {
			doa::camera::y += 3;
		}
		if (doa::keyboard::IsKeyHold(doa::keyboard::A)) {
			doa::camera::x -= 3;
		}
		if (doa::keyboard::IsKeyHold(doa::keyboard::S)) {
			doa::camera::y -= 3;
		}
		if (doa::keyboard::IsKeyHold(doa::keyboard::D)) {
			doa::camera::x += 3;
		}
	}

	void render() {
		if (doa::scene::ACTIVE_SCENE) {
			doa::scene::ACTIVE_SCENE->render();
		}
	}

	void on_window_resize(Window* window, int width, int height) {
		doa::window_width = width;
		doa::window_height = height;
		glViewport(0, 0, width, height);
		calculate_projection_matrix();
	}

	void on_mouse_button_click_and_release(Window* window, int button, int action, int mods) {
		switch (action) {
		case GLFW_RELEASE:
			internal::mouse::press[button] = false;
			internal::mouse::hold[button] = false;
			internal::mouse::release[button] = true;
			break;
		case GLFW_PRESS:
			internal::mouse::press[button] = true;
			internal::mouse::hold[button] = true;
			break;
		}
	}

	void on_mouse_move(Window* window, double xpos, double ypos) {
		doa::mouse::x = (float)xpos - doa::window_width * .5f;
		doa::mouse::y = doa::window_height * .5f - (float)ypos;
	}

	void on_mouse_scroll(Window* window, double xoffset, double yoffset) {
		doa::mouse::z += (float)yoffset * doa::mouse::step_z;
		if (doa::mouse::z < doa::mouse::min_z) {
			doa::mouse::z = doa::mouse::min_z;
		} else if (doa::mouse::z > doa::mouse::max_z) {
			doa::mouse::z = doa::mouse::max_z;
		}
	}

	void on_key_press_and_release(Window* window, int key, int scancode, int action, int mods) {
		switch (action) {
		case GLFW_RELEASE:
			internal::keyboard::press[key] = false;
			internal::keyboard::release[key] = true;
			internal::keyboard::hold[key] = false;
			break;
		case GLFW_PRESS:
			internal::keyboard::press[key] = true;
			internal::keyboard::hold[key] = true;
			break;
		}
	}

	void calculate_view_matrix() {
		m_view_matrix = glm::lookAt(glm::vec3(doa::camera::x, doa::camera::y, 5000), glm::vec3(doa::camera::x, doa::camera::y, 0), glm::vec3(0, 1, 0));
	}

	void calculate_projection_matrix() {
		float w = (float)doa::window_width;
		float h = (float)doa::window_height;
		m_projection_matrix = glm::ortho(w * -.5f, w * .5f, h * -.5f, h * .5f, 0.0001f, 10000.0f);
	}
}