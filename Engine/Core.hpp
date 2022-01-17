#pragma once

#include <memory>

#include <glm/glm.hpp>

struct Angel;
struct Window;
struct FrameBuffer;
struct Project;

struct Core {
	bool _running{ false };
	bool _playing{ false };

	std::unique_ptr<Angel> _angel{ nullptr };
	std::unique_ptr<Window> _window{ nullptr };
	std::unique_ptr<FrameBuffer> _offscreenBuffer{ nullptr };
	Project* _project{ nullptr };

	void LoadProject(const Project& project);
	void UnloadProject();

	void Start();
	void Stop();

	// don't call, call CreateCore to get a core
	Core() = default;

private:
	Core(const Core&) = delete;
	Core(const Core&&) = delete;
	Core& operator=(const Core&) = delete;
	Core& operator=(const Core&&) = delete;

	friend std::unique_ptr<Core>& CreateCore(int width, int height, const char* title, bool isFullscreen, const char* windowIcon, bool renderOffscreen);
	friend void DestroyCore();
};

std::unique_ptr<Core>& CreateCore(int width, int height, const char* title, bool isFullscreen, const char* windowIcon = nullptr, bool renderOffscreen = false);
std::unique_ptr<Core>& GetCore();
void DestroyCore();
