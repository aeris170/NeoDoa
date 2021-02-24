#pragma once

#include <Jinx.hpp>

#include "Core.hpp"

struct JinxEngine {
	Jinx::RuntimePtr _runtime;

private:
	JinxEngine() noexcept;
	~JinxEngine() = default;
	JinxEngine(const JinxEngine&) = delete;
	JinxEngine(const JinxEngine&&) = delete;
	JinxEngine& operator=(const JinxEngine&) = delete;
	JinxEngine& operator=(const JinxEngine&&) = delete;

	friend EXPORT Core* CreateCore(int width, int height, const char* title, bool isFullscreen, bool renderOffscreen);
	//friend EXPORT void DestroyCore(); //keeping it for now
};